#include "test/verification/VerificationRunner.hpp"

#include "simulation/Simulation.hpp"
#include "test/verification/VerificationCaseFactory.hpp"
#include "test/verification/VerificationIO.hpp"
#include "test/verification/ErrorMetrics.hpp"
#include "test/verification/VerificationSummary.hpp"
#include "test/verification/VerificationAnalyzer.hpp"

#include "config/PathContext.hpp"
#include "config/SimulationConfig.hpp"
#include "mesh/MeshBase.hpp"

#include "io/PlotUtils.hpp"
#include "io/OutputData.hpp"
#include "io/OutputManager.hpp"
#include "io/OutputBuilder.hpp"

#include "diagnostics/MemoryDiagnostics.hpp"

#include "utils/Timer.hpp"
#include "fields/FieldNames.hpp"

#include "equation_systems/Residual.hpp"

#include <iostream>
#include <filesystem>
#include <iomanip>
#include <cmath>

#include "test/verification/VerificationTools.hpp"

SimulationConfig VerificationRunner::applyVerificationOverrides(
    const SimulationConfig& base,
    const VerificationCaseConfig& verif)
{
    SimulationConfig cfg = base;

    if (verif.overrideMesh) { cfg.mesh = verif.mesh; }
    if (verif.overridePhysics) { cfg.physics = verif.physics; }
    if (verif.overrideSolver) { cfg.solver = verif.solver; }
    if (verif.overrideBoundary) { cfg.boundary = verif.boundary; }
    if (verif.overrideDiscretization) { cfg.discretization = verif.discretization; }

    return cfg;
}

VerificationCaseConfig VerificationRunner::loadVerificationCase( const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { throw std::runtime_error( "Cannot open verification case: " + path.string()); }

    nlohmann::json j;
    file >> j;


    // Handle extends
    if (j.contains("extends"))
    {
        auto parent =std::filesystem::weakly_canonical(path.parent_path() /j["extends"].get<std::string>());

        std::ifstream baseFile(parent);

        nlohmann::json base;
        baseFile >> base;

        j = mergeJson(base, j);
    }


    if (!j.contains("verificationCase")) { throw std::runtime_error("Missing verificationCase block"); }


    return j["verificationCase"].get<VerificationCaseConfig>();
}

void VerificationRunner::run( const SimulationConfig& baseCfg, const VerificationSuite& suite, const PathContext& paths)
{
    Timer verificationTimer("Verification Suite");
    if (!suite.enabled) { return; }

    std::cout << "\n================ VERIFICATION MODE ================\n";
    std::cout << "Verification enabled: " << suite.enabled << "\n";
    std::cout << "Plot enabled: " << suite.plot_enabled << "\n";
    std::cout << "Case count: " << suite.cases.size() << "\n";

    const auto& verificationRoot = paths.verificationRoot;
    
    std::filesystem::create_directories(verificationRoot);

    // -------------------------------------------------
    // Main verification loop (NOW FULLY TYPED)
    // -------------------------------------------------
    
    std::vector<VerificationSummary> summary;
    Point qoi_eval_point;
    qoi_eval_point.x[0] = 0.2676;

    for (const auto& caseEntry : suite.cases)
    {
        // Timer caseTimer("Verification Case: " + caseEntry.name);
        const std::string caseName = caseEntry.name;

        // -------------------------------------------------
        // Load individual verification case file
        // -------------------------------------------------

        auto casePath = std::filesystem::path(suite.case_directory) / (caseName + ".json");

        std::cout << "Loading verification case: " << casePath << "\n";

        if (!std::filesystem::exists(casePath)) { throw std::runtime_error( "Missing verification case file: " + casePath.string()); }

        auto verificationCase = loadVerificationCase(casePath);

        SimulationConfig caseCfg = applyVerificationOverrides(baseCfg, verificationCase);
        
        auto caseOutputDir = verificationRoot / caseName;

        std::filesystem::create_directories(caseOutputDir);

        std::cout << "\n=================================\n"
                << "Running verification case: "
                << caseName
                << "\n=================================\n";

        // -------------------------------------------------
        // Build manufactured / verification case
        // -------------------------------------------------

        bool refinementEnabled = verificationCase.refinement.enabled;
        const auto& refinementMeshSizes = verificationCase.refinement.levels;
        double expectedOrder = verificationCase.refinement.expected_order;
        std::vector<RefinementLevel> refinementLevels;
        VerificationSummary finestSummary;     

        const std::size_t nLevels = refinementEnabled ? refinementMeshSizes.size() : 1;

        if (refinementEnabled && refinementMeshSizes.size() < 2)
        {
            throw std::runtime_error( "Refinement is enabled for case '" + caseName + "' but fewer than two refinement levels were provided.");
        }

        for (std::size_t level = 0; level < nLevels; ++level)
        {
            SimulationConfig levelCfg = caseCfg;
            // Timer levelTimer(  "  Level " + std::to_string(level) +  " (" + std::to_string(levelCfg.mesh.nx) + "x" + std::to_string(levelCfg.mesh.ny) + ")" );
            
            if (refinementEnabled)
            {
                levelCfg.mesh.nx = refinementMeshSizes[level];

                if (levelCfg.mesh.type == "quad2D") { levelCfg.mesh.ny = refinementMeshSizes[level]; } 
            }

            std::cout << "Creating simulation:"
                        << "\n  mesh = " << levelCfg.mesh.type
                        << "\n  nx = " << levelCfg.mesh.nx
                        << "\n  ny = " << levelCfg.mesh.ny
                        << "\n  physics = " << physics::to_string(levelCfg.physics.type)
                        << "\n";

            const auto levelStartMemory = MemoryDiagnostics::currentRSS();

            Simulation sim(levelCfg);

            auto casePtr = VerificationCaseFactory::create(caseName, levelCfg);

            sim.setVerificationCase(std::move(casePtr));

            const MeshBase& mesh = sim.mesh();
            sim.verificationCase()->initialize(mesh); 
            
            {
                // Timer timer("Assembly");
                sim.assemble();
            }

            const auto afterAssemblyMemory = MemoryDiagnostics::currentRSS();

            {
                // Timer timer("Solver");
                sim.solve();
            }

            {
                const auto afterSolveMemory = MemoryDiagnostics::currentRSS(); 
                const auto peakMemory = MemoryDiagnostics::peakRSS(); 
                const double levelStartMB = static_cast<double>(levelStartMemory) / (1024.0 * 1024.0); 
                const double assemblyMB = static_cast<double>(afterAssemblyMemory) / (1024.0 * 1024.0); 
                const double solveMB = static_cast<double>(afterSolveMemory) / (1024.0 * 1024.0); 
                const double peakMB = static_cast<double>(peakMemory) / (1024.0 * 1024.0); 
                const double peakIncreaseMB = static_cast<double>(peakMemory - levelStartMemory) / (1024.0 * 1024.0); 
                std::cout << "\n================ MEMORY DIAGNOSTICS ================\n" 
                          << "Start RSS:           "  << levelStartMB                 << " MB\n" 
                          << "After assembly:      "  << assemblyMB                   << " MB\n"
                          << "After solve:         "  << solveMB                      << " MB\n"
                          
                          << "\nAssembly Increase:   "  << (assemblyMB - levelStartMB)  << " MB\n"
                          << "Solver Increase:     "  << (solveMB - assemblyMB)       << " MB\n"
                          << "Peak RSS:            "  << peakMB                       << " MB\n" 
                          << "Peak increase:       "  << peakIncreaseMB               << " MB\n" 
                          << "====================================================\n\n";
            }   

            auto& solution = sim.solution();

            const double qoiValue = solution(qoi_eval_point);
            // std::cout << "Level " << level << ": QoI = " << qoiValue << "\n";

            // -------------------------------------------------
            // Exact solution evaluation
            // -------------------------------------------------
            ScalarField exactField(
                "ExactSolution",
                mesh,
                FieldLocation::Cell,
                0.0
            );

            if (!sim.verificationCase()) 
            { 
                throw std::runtime_error( "Verification enabled but no verification case attached." ); 
            }

            const auto& verifCase = *sim.verificationCase();
            const double exactQoi = verifCase.exact(qoi_eval_point);
            const double qoiError = std::abs(qoiValue - exactQoi);

            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const auto& xc = mesh.cellCenter(c);

                exactField[c] = verifCase.exact(xc);       
                // This should change such that exact() accepts points, not x,y,z. This makes it dimension agnositc.
            }

            // -------------------------------------------------
            // Error norms
            // -------------------------------------------------
            auto norms = ErrorNorms::compute(mesh, solution, exactField);
            double hx = levelCfg.mesh.lx / levelCfg.mesh.nx;
            double hy = levelCfg.mesh.ly / levelCfg.mesh.ny;

            double h = 0.0;
            if (levelCfg.mesh.type == "line1D") { h = hx; }
            else if (levelCfg.mesh.type == "quad2D") { h = std::max(hx, hy); }

            refinementLevels.push_back({
                levelCfg.mesh.nx,
                levelCfg.mesh.ny,
                h,
                norms.l2_rms,
                norms.linf,
                qoiValue,
                qoiError
            });

            std::vector<double> residual;

            {
                // Timer timer("Residual calculation");
                residual = computeResidual(sim.system(), solution);
            }

            // DEBUGGING
            std::cout << "REFINEMENT DATA: "
                    << levelCfg.mesh.nx
                    << "x"
                    << levelCfg.mesh.ny
                    << " h="
                    << h
                    << " L2="
                    << norms.l2_rms
                    << " Linf="
                    << norms.linf
                    << " qoi="
                    << qoiValue
                    << " qoiError="
                    << qoiError
                    << "\n";

            // -------------------------------------------------
            // Output paths
            // -------------------------------------------------
            std::filesystem::path levelOutputDir = caseOutputDir;

            if (refinementEnabled) { levelOutputDir /= "L" + std::to_string(level + 1); }

            std::filesystem::create_directories(levelOutputDir);
            // all .csv files are named solution.csv to simplify plotting. They are nested under their test, so this shouldn't be confusing.
            auto csvPath = levelOutputDir / ("solution.csv");       
            auto vtkPath = levelOutputDir / (caseName + ".vtu");
            // auto jsonPath = levelOutputDir / (caseName + ".json");
            // -------------------------------------------------
            // Write outputs
            // -------------------------------------------------
            {                
                auto output =
                    OutputBuilder::build(
                        sim,
                        solution,
                        residual);

                OutputManager::write(
                    output,
                    levelOutputDir);

                // VerificationIO::writeReport(
                //     finestSummary,
                //     jsonPath);
            }
            
            // -------------------------------------------------
            // Plotting
            // -------------------------------------------------
            {
                if (suite.plot_enabled) { runPlot(paths, csvPath); }
            }

            // -------------------------------------------------
            // Report
            // -------------------------------------------------
            std::cout << "\n================ VERIFICATION ================\n"
                    << "Case      : " << caseName  << "\n"
                    << "L2 Norm   : " << norms.l2_rms << "\n"
                    << "Linf Norm : " << norms.linf << "\n"
                    << "CSV Output: " << csvPath << "\n"
                    // << "JSON Output: " << jsonPath << "\n"
                    << "VTK Output: " << vtkPath << "\n"
                    << "=============================================\n";
            
            std::string meshType = caseCfg.mesh.type;

            std::string meshSize;

            if (meshType == "line1D") { meshSize = std::to_string(caseCfg.mesh.nx) + "x1"; }
            else if (meshType == "quad2D") { meshSize = std::to_string(caseCfg.mesh.nx) + "x" + std::to_string(caseCfg.mesh.ny); }
            else { meshSize = std::to_string(mesh.ncells()) + " cells"; }

            double l2_tol = sim.verificationCase()->l2AcceptanceThreshold();
            double linf_tol = sim.verificationCase()->linfAcceptanceThreshold();

            bool passed = norms.l2_rms <= l2_tol && norms.linf <= linf_tol;

            if (level == nLevels - 1)                       // Adjusted to be last level, not first.
            {
                finestSummary.caseName = caseName;
                finestSummary.solver = solver::to_string(levelCfg.solver.method);
                finestSummary.meshType = meshType;
                finestSummary.meshSize = meshSize;
                finestSummary.gradient = gradientToString(levelCfg.discretization.gradientScheme);
                // std::cout << "Reconstruction scheme: " << reconstructionToString(levelCfg.discretization.reconstructionScheme) << '\n';
                finestSummary.reconstruction = reconstructionToString(levelCfg.discretization.reconstructionScheme);

                finestSummary.l2Error = norms.l2_rms;
                finestSummary.linfError = norms.linf;
                finestSummary.qoiValue = qoiValue;

                finestSummary.l2AcceptanceTol = l2_tol;
                finestSummary.linfAcceptanceTol = linf_tol;

                finestSummary.accuracyPassed = passed;

                finestSummary.refinementEnabled = refinementEnabled;
                finestSummary.refinementPassed = false;
                finestSummary.l2Order = 0.0;
                finestSummary.linfOrder = 0.0;
            }
        }

        if (refinementEnabled)
        {
            GridConvergenceStudy study =
                VerificationAnalyzer::analyzeRefinement(refinementLevels);

            constexpr double orderTolerance = 0.1;

            const double l2Order = study.l2Regression.slope;
            const double linfOrder = study.linfRegression.slope;
            const double qoiOrder = study.qoiRegression.slope;
            const double qoiRichardson = study.qoiRichardson;

            const bool refinementPassed =
                std::abs(l2Order - expectedOrder) <= orderTolerance &&
                std::abs(linfOrder - expectedOrder) <= orderTolerance;

            finestSummary.refinementPassed = refinementPassed;
            finestSummary.l2Order = l2Order;
            finestSummary.linfOrder = linfOrder;
            finestSummary.qoiOrder = qoiOrder;

            std::cout
                << "\n================ REFINEMENT STUDY ================\n"
                << "Case: " << caseName << "\n"
                << "Observed L2 Order   : " << l2Order << "\n"
                << "Observed Linf Order : " << linfOrder << "\n"
                << "Expected Order      : " << expectedOrder << "\n"
                << "Status              : "
                << (refinementPassed ? "PASS" : "FAIL")
                << "\n"
                << "Observed QoI Order  : " << qoiOrder << "\n"
                << "Richardson QOI      : " << qoiRichardson << "\n"
                << "==================================================\n";
        }
    
        const auto jsonPath = caseOutputDir / (caseName + ".json");

        VerificationIO::writeReport(
            finestSummary,
            jsonPath);

        std::cout << "JSON Output: " << jsonPath << "\n";

        summary.push_back(finestSummary);
    }
    std::size_t l2PassedCount = 0;
    std::size_t refinementPassedCount = 0;

    std::size_t l2Total = summary.size();
    std::size_t refinementTotal = 0;

    for (const auto& s : summary)
    {
        if (s.accuracyPassed) { ++l2PassedCount; }

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
            << std::setw(22) << "Case"
            << std::setw(10) << "Solver"
            << std::setw(18) << "Reconstruction"
            << std::setw(16) << "Gradient"
            << std::setw(10) << "Mesh"
            << std::setw(12) << "L2 Error"
            << std::setw(10) << "Accuracy"
            << std::setw(14) << "Convergence"
            << std::setw(12) << "L2 Order"
            << std::setw(13) << "Linf Order"
            << std::setw(10) << "QoI"
            << "\n";

    std::cout << std::string(146, '-') << "\n";

    std::cout << std::scientific << std::setprecision(3);

    for (const auto& s : summary)
    {
        std::cout << std::left
                << std::setw(22) << s.caseName
                << std::setw(10) << s.solver
                << std::setw(18) << s.reconstruction
                << std::setw(16) << s.gradient
                << std::setw(10) << s.meshSize
                << std::setw(12) << s.l2Error
                << std::setw(10) << (s.accuracyPassed ? "PASS" : "FAIL");

        if (s.refinementEnabled)
        {
            std::cout << std::setw(14)
                    << (s.refinementPassed ? "PASS" : "FAIL")
                    << std::setw(12)
                    << s.l2Order
                    << std::setw(13)
                    << s.linfOrder
                    << std::setw(10)
                    << s.qoiValue;
        }
        else
        {
            std::cout << std::setw(14)
                    << "--"
                    << std::setw(12)
                    << "--"
                    << std::setw(13)
                    << "--"
                    << std::setw(10)
                    << s.qoiValue;
        }

        std::cout << "\n";
    }
    // std::cout << "\nVerification Result: " << passedCount << "/" << summary.size() << " cases passed, " << summary.size() - passedCount << " failed\n\n";
    std::cout << "\n================ FINAL RESULT ================\n";
    std::cout
        << "Accuracy Checks : "
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
