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

// Verficiation Case Dictionary
struct VerificationSummary
{
    std::string caseName;
    std::string solver;
    std::string meshType;
    std::string meshSize;

    double l2;
    double linf;

    double l2Tol;
    double linfTol;

    // Individual verification checks
    bool l2Passed = false;

    bool refinementEnabled = false;
    bool refinementPassed = false;
    double observedOrder = 0.0;

    // Overall case status
    bool passed() const
    {
        if (refinementEnabled)
        {
            return refinementPassed;
        }

        return l2Passed;
    }
};

// Mesh Refinement studies
struct RefinementLevel
{
    std::size_t nx;
    std::size_t ny;

    double h;

    double l2;
    double linf;
};

struct RefinementSummary
{
    std::string caseName;

    std::vector<RefinementLevel> levels;

    double observedOrderL2 = 0.0;
    double observedOrderLinf = 0.0;

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

    auto verificationRoot = paths.outputRoot / "validation";

    std::filesystem::create_directories(verificationRoot);

    // -------------------------------------------------
    // Main verification loop (NOW FULLY TYPED)
    // -------------------------------------------------
    
    std::vector<VerificationSummary> summary;
    std::vector<RefinementSummary> refinementSummary;

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

        bool refinementEnabled = caseEntry.refinement.enabled;
        const auto& refinementLevels = caseEntry.refinement.levels;
        double expectedOrder = caseEntry.refinement.expected_order;

        RefinementSummary refinement;
        refinement.caseName = caseName;

        const int nLevels = refinementEnabled ? refinementLevels.size() : 1;

        for (int level = 0; level < nLevels; ++level)
        {
            SimulationConfig levelCfg = caseCfg;

            if (refinementEnabled)
            {
                levelCfg.mesh.nx = refinementLevels[level];

                if (levelCfg.mesh.type == "quad2D") { levelCfg.mesh.ny = refinementLevels[level]; }
            }

            std::cout << "Creating simulation:"
                        << "\n  mesh = " << levelCfg.mesh.type
                        << "\n  nx = " << levelCfg.mesh.nx
                        << "\n  ny = " << levelCfg.mesh.ny
                        << "\n  physics = " << physics::to_string(levelCfg.physics.type)
                        << "\n";

            Simulation sim(levelCfg);

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

                double leftBC  = levelCfg.boundary.at(0).condition.value;
                double rightBC = levelCfg.boundary.at(1).condition.value;

                if (std::abs(leftBC - TL) > 1e-12) { throw std::runtime_error( "Quadratic1D: TL does not match left boundary." ); }

                if (std::abs(rightBC - TR) > 1e-12) { throw std::runtime_error( "Quadratic1D: TR does not match right boundary." ); }
            }

            // -------------------------------------------------
            // Exact solution evaluation
            // -------------------------------------------------
            std::vector<double> exactField(mesh.ncells());

            if (!sim.verificationCase()) { throw std::runtime_error( "Verification enabled but no verification case attached." ); }

            const auto& verifCase = *sim.verificationCase();

            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const auto& xc = mesh.cellCenter(c);

                exactField[c] = verifCase.exact(xc.x[0], xc.x[1]);
            }

            // -------------------------------------------------
            // Error norms
            // -------------------------------------------------
            auto norms = ErrorNorms::compute(mesh, phi, exactField);
            double hx = levelCfg.mesh.lx / levelCfg.mesh.nx;
            double hy = levelCfg.mesh.ly / levelCfg.mesh.ny;

            double h = 0.0;
            if (levelCfg.mesh.type == "Line1D") { h = hx; }
            else if (levelCfg.mesh.type == "quad2D") { h = std::max(hx, hy); }

            refinement.levels.push_back({
                levelCfg.mesh.nx,
                levelCfg.mesh.ny,
                h,
                norms.l2_rms,
                norms.linf
            });

            // -------------------------------------------------
            // Output paths
            // -------------------------------------------------
            std::string suffix;

            if (refinementEnabled) { suffix = "_L" + std::to_string(level); }

            auto csvPath = verificationRoot / (caseName + suffix + ".csv");
            auto jsonPath = verificationRoot / (caseName + suffix + ".json");

            // -------------------------------------------------
            // Write outputs
            // -------------------------------------------------
            VerificationIO::writeCSV(sim, phi, csvPath);
            VerificationIO::writeSummary(
                caseName + suffix,
                norms.l2_rms,
                norms.linf,
                jsonPath
            );

            // -------------------------------------------------
            // Plotting
            // -------------------------------------------------
            if (cfg.verificationSuite.plot_enabled) {
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
            
            std::string meshType = caseCfg.mesh.type;

            std::string meshSize;

            if (meshType == "line1D") { meshSize = std::to_string(caseCfg.mesh.nx) + "x1"; }
            else if (meshType == "quad2D") { meshSize = std::to_string(caseCfg.mesh.nx) + "x" + std::to_string(caseCfg.mesh.ny); }
            else { meshSize = std::to_string(mesh.ncells()) + " cells"; }

            double l2_tol = sim.verificationCase()->l2AcceptanceThreshold();
            double linf_tol = sim.verificationCase()->linfAcceptanceThreshold();

            bool passed = norms.l2_rms <= l2_tol && norms.linf <= linf_tol;

            if (level == 0)
            {
                summary.emplace_back(VerificationSummary{
                    caseName,
                    solver::to_string(levelCfg.solver.method),
                    meshType,
                    meshSize,
                    norms.l2_rms,
                    norms.linf,
                    l2_tol,
                    linf_tol,
                    passed,
                    refinementEnabled,
                    false,
                    0.0
                });
            }
        }

        if (refinementEnabled)  
        { 
            for (std::size_t i = 1; i < refinement.levels.size(); ++i)
            {
                const auto& coarse = refinement.levels[i-1];
                const auto& fine   = refinement.levels[i];

                double orderL2 = std::log(coarse.l2 / fine.l2) / std::log(coarse.h / fine.h);

                double orderLinf = std::log(coarse.linf / fine.linf) / std::log(coarse.h / fine.h);

                if (fine.l2 > 0.0 && coarse.l2 > 0.0)
                {
                    double orderL2 = std::log(coarse.l2 / fine.l2) / std::log(coarse.h / fine.h);
                    refinement.observedOrderL2 += orderL2;
                }
                if (fine.linf > 0.0 && coarse.linf > 0.0)
                {
                    double orderLinf = std::log(coarse.linf / fine.linf) / std::log(coarse.h / fine.h);
                    refinement.observedOrderLinf += orderLinf;
                }

                refinement.observedOrderL2 += orderL2;
                refinement.observedOrderLinf += orderLinf;
            }

            if (refinement.levels.size() > 1) { 
                double n = refinement.levels.size() - 1; 
            
                refinement.observedOrderL2 /= n;
                refinement.observedOrderLinf /= n;

                refinement.passed = refinement.observedOrderL2 >= expectedOrder && refinement.observedOrderLinf >= expectedOrder;
            }
            else { refinement.passed = false; }

            for (auto& s : summary)
            {
                if (s.caseName == caseName && s.refinementEnabled)
                {
                    s.refinementPassed = refinement.passed;
                    s.observedOrder = refinement.observedOrderL2;
                }
            }

            refinementSummary.push_back(std::move(refinement));

            std::cout 
                << "\n================ REFINEMENT STUDY ================\n"
                << "Case: " << refinement.caseName << "\n"
                << "Observed L2 Order   : " << refinement.observedOrderL2 << "\n"
                << "Observed Linf Order : " << refinement.observedOrderLinf << "\n"
                << "Expected Order      : " << expectedOrder << "\n"
                << "Status              : "
                << (refinement.passed ? "PASS" : "FAIL")
                << "\n"
                << "==================================================\n";
        }
    }

    std::size_t l2PassedCount = 0;
    std::size_t refinementPassedCount = 0;

    std::size_t l2Total = summary.size();
    std::size_t refinementTotal = 0;

    for (const auto& s : summary)
    {
        if (s.l2Passed) { ++l2PassedCount; }

        if (s.refinementEnabled)
        {
            ++refinementTotal;

            if (s.refinementPassed) { ++refinementPassedCount; }
        }
    }

    std::size_t totalChecks = l2Total + refinementTotal;

    std::size_t passedChecks = l2PassedCount + refinementPassedCount;

    std::cout << "\n================================================================================\n";
    std::cout << "Verification Summary\n";
    std::cout << "================================================================================\n\n";

    std::cout << std::left
            << std::setw(24) << "Case"
            << std::setw(12) << "Solver"
            << std::setw(12) << "Mesh"
            << std::setw(14) << "L2 Error"
            << std::setw(12) << "Accuracy"
            << std::setw(14) << "Convergence"
            << std::setw(10) << "Order"
            << "\n";

    std::cout << std::string(98, '-') << "\n";

    std::cout << std::scientific << std::setprecision(3);

    for (const auto& s : summary)
    {
        std::cout << std::left
                << std::setw(24) << s.caseName
                << std::setw(12) << s.solver
                << std::setw(12) << s.meshSize
                << std::setw(14) << s.l2
                << std::setw(12) << (s.l2Passed ? "PASS" : "FAIL");

        if (s.refinementEnabled)
        {
            std::cout << std::setw(14)
                    << (s.refinementPassed ? "PASS" : "FAIL")
                    << std::setw(10)
                    << s.observedOrder;
        }
        else
        {
            std::cout << std::setw(14)
                    << "N/A"
                    << std::setw(10)
                    << "N/A";
        }

        std::cout << "\n";
    }
    // std::cout << "\nVerification Result: " << passedCount << "/" << summary.size() << " cases passed, " << summary.size() - passedCount << " failed\n\n";
    std::cout << "\n================ FINAL RESULT ================\n";
    std::cout
        << "L2 Accuracy Checks : "
        << l2PassedCount << "/" << l2Total
        << "\n";

    std::cout
        << "Mesh Refinement Checks : "
        << refinementPassedCount << "/" << refinementTotal
        << "\n";

    std::cout
        << "Overall Checks Passed: "
        << passedChecks << "/" << totalChecks
        << "\n";
}
