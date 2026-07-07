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

        sim.assemble();
        auto phi = sim.solve();

        const MeshBase& mesh = sim.mesh();
        // const auto& verifCase = *sim.verificationCase();

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

        // -------------------------------------------------
        // Error norms
        // -------------------------------------------------
        auto norms = ErrorNorms::compute(mesh, phi, exactField);

        std::cout << cfg.physics.k << "\n";
        std::cout << cfg.mesh.nx << "\n";

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
        VerificationIO::writeSummary( caseName, norms.l2_energy, norms.linf, jsonPath );

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
                  << "L2 Norm   : " << norms.l2_energy << "\n"
                  << "Linf Norm : " << norms.linf << "\n"
                  << "CSV Output: " << csvPath << "\n"
                  << "JSON Output: " << jsonPath << "\n"
                  << "=============================================\n";
    }
}
