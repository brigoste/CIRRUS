#include "tests/verification/VerificationRunner.hpp"

#include "simulation/Simulation.hpp"
#include "tests/verification/VerificationCaseFactory.hpp"
#include "tests/verification/VerificationIO.hpp"
#include "tests/verification/ErrorMetrics.hpp"

#include "config/PathContext.hpp"
#include "mesh/MeshBase.hpp"
#include "io/PlotUtils.hpp"

#include <iostream>
#include <filesystem>
#include <iomanip>

struct VerificationSummary
{
    std::string caseName;
    std::string solver;

    std::size_t nx;
    std::size_t ny;

    double l2;
    double linf;

    bool passed = true;
};


void VerificationRunner::run(
    const SimulationConfig& cfg,
    const PathContext& paths)
{
    if (!cfg.verificationSuite.enabled) { return; }

    std::cout << "\n================ VERIFICATION MODE ================\n";
    std::cout << "Verification enabled: " << cfg.verificationSuite.enabled << "\n";
    std::cout << "Plot enabled: " << cfg.verificationSuite.plot_enabled << "\n";
    std::cout << "Case count: " << cfg.verificationSuite.cases.size() << "\n";

    std::filesystem::create_directories(paths.outputRoot);

    // -------------------------------------------------
    // Main verification loop (NOW FULLY TYPED)
    // -------------------------------------------------
    
    std::vector<VerificationSummary> summary;

    for (const auto& caseEntry : cfg.verificationSuite.cases)
    {
        const std::string& caseName = caseEntry.name;
        // const nlohmann::json& params = caseEntry.params;

        std::cout << "\n=================================\n"
                  << "Running verification case: " << caseName
                  << "\n=================================\n";

        // -------------------------------------------------
        // Build manufactured / verification case
        // -------------------------------------------------
        SimulationConfig caseCfg = resolveCaseConfig(cfg, caseEntry);

        Simulation sim(caseCfg);

        auto casePtr = VerificationCaseFactory::create(caseName, caseEntry.params);

        sim.setVerificationCase(std::move(casePtr));
        
        const MeshBase& mesh = sim.mesh();
        sim.verificationCase()->initialize(mesh); 
        
        sim.assemble();
        
        auto phi = sim.solve();             

        if (caseEntry.name == "Quadratic1D")
        {
            const double TL = caseEntry.params.at("TL").get<double>();
            const double TR = caseEntry.params.at("TR").get<double>();

            double leftBC  = caseCfg.boundary.at(0).condition.value;
            double rightBC = caseCfg.boundary.at(1).condition.value;

            if (std::abs(leftBC - TL) > 1e-12) { throw std::runtime_error( "Quadratic1D: TL does not match left boundary." ); }

            if (std::abs(rightBC - TR) > 1e-12) { throw std::runtime_error( "Quadratic1D: TR does not match right boundary." ); }
        }

        // -------------------------------------------------
        // Exact solution evaluation
        // -------------------------------------------------
        std::vector<double> exactField(mesh.ncells());

        if (!sim.verificationCase())
        {
            throw std::runtime_error(
                "Verification enabled but no verification case attached."
            );
        }

        const auto& verifCase = *sim.verificationCase();

        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            const auto& xc = mesh.cellCenter(c);

            exactField[c] = verifCase.exact(xc.x[0], xc.x[1]);

            // std::cout
            //     << c
            //     << " x=" << xc.x[0]
            //     << " phi=" << phi[c]
            //     << " exact=" << exactField[c]
            //     << " error=" << phi[c]-exactField[c]
            //     << "\n";
        }

        // -------------------------------------------------
        // Error norms
        // -------------------------------------------------
        auto norms = ErrorNorms::compute(mesh, phi, exactField);

        // -------------------------------------------------
        // Output paths
        // -------------------------------------------------
        auto csvPath  = paths.outputRoot / (caseName + ".csv");
        auto jsonPath = paths.outputRoot / (caseName + ".json");

        std::filesystem::create_directories(paths.outputRoot);

        // -------------------------------------------------
        // Write outputs
        // -------------------------------------------------
        VerificationIO::writeCSV(sim, phi, csvPath);
        VerificationIO::writeSummary( caseName, norms.l2_rms, norms.linf, jsonPath );

        // -------------------------------------------------
        // Plotting
        // -------------------------------------------------
        if (cfg.verificationSuite.plot_enabled) 
        {
            std::cout << "Plotting " << caseName << " from " << csvPath << "\n";
            runPlot(csvPath.generic_string());
        }

        // -------------------------------------------------
        // Report
        // -------------------------------------------------
        std::cout << "\n================ VERIFICATION ================\n"
                  << "Case      : " << caseName << "\n"
                  << "L2 Norm   : " << norms.l2_rms << "\n"
                  << "Linf Norm : " << norms.linf << "\n"
                  << "CSV Output: " << csvPath << "\n"
                  << "JSON Output: " << jsonPath << "\n"
                  << "=============================================\n";


        summary.emplace_back(VerificationSummary{
            caseName,
            solver::to_string(caseCfg.solver.method),
            caseCfg.mesh.nx,
            caseCfg.mesh.ny,
            norms.l2_rms,
            norms.linf,
            (norms.l2_rms < 1e-6)                // <-------------------------------------- Set "passed" parameter hack
        });
    }
    std::cout << "\n==============================================================\n";
    std::cout << "Verification Summary\n";
    std::cout << "==============================================================\n\n";

    std::cout << std::left
          << std::setw(24) << "Case"
          << std::setw(12) << "Solver"
          << std::left
          << std::setw(8)  << "Nx"
          << std::setw(8)  << "Ny"
          << std::setw(15) << "L2 RMS"
          << std::setw(15) << "Linf"
          << std::setw(8)  << "Status"
          << "\n";

    std::cout << std::string(90, '-') << "\n";

    std::cout << std::scientific << std::setprecision(3);
    for (const auto& s : summary)
    {
        std::cout << std::left
                << std::setw(24) << s.caseName
                << std::setw(12) << s.solver
                << std::setw(8)  << s.nx
                << std::setw(8)  << s.ny
                << std::setw(15) << std::scientific << std::setprecision(6) << s.l2
                << std::setw(15) << s.linf
                << std::setw(8)  << (s.passed ? "PASS" : "FAIL")
                << "\n";
    }
}
