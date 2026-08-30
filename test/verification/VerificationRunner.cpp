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

#include "utils/Timer.hpp"

#include "equation_systems/Residual.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>

SimulationConfig VerificationRunner::applyVerificationOverrides(
    const SimulationConfig& base,
    const VerificationCaseConfig& verif)
{
    SimulationConfig cfg = base;

    if (verif.overrideMesh)           { cfg.mesh = verif.mesh; }
    if (verif.overridePhysics)        { cfg.physics = verif.physics; }
    if (verif.overrideSolver)         { cfg.solver = verif.solver; }
    if (verif.overrideBoundary)       { cfg.boundary = verif.boundary; }
    if (verif.overrideDiscretization) { cfg.discretization = verif.discretization; }
    if (verif.overrideAcceptance)     { cfg.acceptance = verif.acceptance; }

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

bool VerificationRunner::run( const SimulationConfig& baseCfg, const VerificationSuite& suite, const PathContext& paths)
{
    Timer verificationTimer("Verification Suite");
    if (!suite.enabled) { return true; }

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
    qoi_eval_point.x[1] = 0.5;

    for (const auto& caseEntry : suite.cases)
    {
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

        double exactQoi = 0.0;

        for (std::size_t level = 0; level < nLevels; ++level)
        {
            SimulationConfig levelCfg = caseCfg;

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

            Simulation sim(levelCfg);

            auto casePtr = VerificationCaseFactory::create(caseName, levelCfg);

            sim.setVerificationCase(std::move(casePtr));

            if (!sim.verificationCase()) 
            { 
                throw std::runtime_error( "Verification enabled but no verification case attached." ); 
            }

            const MeshBase& mesh = sim.mesh();
            sim.verificationCase()->initialize(mesh); 

            sim.assemble();

            // VVVVVVVVVVVVVVVVVV DEBUG SECTION VVVVVVVVVVVVVVVVVV

            std::vector<double> constantSolution( sim.system().size(), 400.0 );

            auto constantResidual = computeResidual( sim.system(), constantSolution );
            double maxResidual = 0.0;
            std::size_t maxCell = 0;

            for (std::size_t i = 0; i < constantResidual.size(); ++i)
            {
                const double magnitude = std::abs(constantResidual[i]);

                if (magnitude > maxResidual)
                {
                    maxResidual = magnitude;
                    maxCell = i;
                }
            }

            const auto& maxCenter = sim.mesh().cellCenter(maxCell);

            std::cout << "Max residual cell = " << maxCell << '\n'
                      << "Cell center       = ("
                      << maxCenter.x[0] << ", "
                      << maxCenter.x[1] << ")\n"
                      << "Residual          = " << constantResidual[maxCell] << '\n'
                      << "Abs residual      = " << std::abs(constantResidual[maxCell]) << '\n';

            double l2Residual = 0.0;

            for (double r : constantResidual)
            {
                maxResidual = std::max(maxResidual, std::abs(r));
                l2Residual += r * r;
            }

            l2Residual = std::sqrt(l2Residual);

            std::size_t badCells = 0;

            for (double r : constantResidual)
            {
                if (std::abs(r) > 1e-10) { ++badCells; }
            }

            std::cout << "Cells with |residual| > 1e-10: "
                      << badCells << " / "
                      << constantResidual.size()
                      << '\n';
              
            std::cout << "\n================ CONSTANT SOLUTION TEST ================\n"
                      << "Assumed solution : T = 400\n"
                      << "Residual L2      : " << l2Residual << '\n'
                      << "Residual Linf    : " << maxResidual << '\n'
                      << "=========================================================\n";

            // ^^^^^^^^^^^^^^^^^ DEBUG SECTION ^^^^^^^^^^^^^^^^^

            sim.solve();

            auto& solution = sim.solution();

            const double qoiValue = solution(qoi_eval_point);

            // -------------------------------------------------
            // Exact solution evaluation
            // -------------------------------------------------
            ScalarField exactField(
                "ExactSolution",
                mesh,
                FieldLocation::Cell,
                0.0
            );

            const auto& verifCase = *sim.verificationCase();
            const double exactQoiAtLevel = verifCase.exact(qoi_eval_point);
            const double qoiError = std::abs(qoiValue - exactQoiAtLevel);

            if (level == nLevels - 1) { exactQoi = exactQoiAtLevel; }

            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const auto& xc = mesh.cellCenter(c);

                exactField[c] = verifCase.exact(xc);       
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
            residual = computeResidual(sim.system(), solution);

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
                      << "\n\n-----------------------------------\n\n";

            // -------------------------------------------------
            // Output paths
            // -------------------------------------------------
            std::filesystem::path levelOutputDir = caseOutputDir;

            if (refinementEnabled) { levelOutputDir /= "L" + std::to_string(level + 1); }

            std::filesystem::create_directories(levelOutputDir);
            // all .csv files are named solution.csv to simplify plotting. They are nested under their test, so this shouldn't be confusing.
            auto csvPath = levelOutputDir / ("solution.csv");       
            
            // -------------------------------------------------
            // Write outputs
            // -------------------------------------------------
            auto output =
                OutputBuilder::build(
                    sim,
                    solution,
                    residual);

            OutputManager::write(
                output,
                levelOutputDir);
            
            // -------------------------------------------------
            // Plotting
            // -------------------------------------------------
            if (suite.plot_enabled) { runPlot(paths, csvPath); }
            
            const std::string meshType = levelCfg.mesh.type;
            std::string meshSize;

            if (meshType == "line1D")      { meshSize = std::to_string(levelCfg.mesh.nx) + "x1"; }
            else if (meshType == "quad2D") { meshSize = std::to_string(levelCfg.mesh.nx) + "x" + std::to_string(levelCfg.mesh.ny); }
            else                           { meshSize = std::to_string(mesh.ncells()) + " cells"; }

            

            if (level == nLevels - 1)                       // Adjusted to be last level, not first.
            {
                const double l2_tol   = sim.verificationCase()->l2AcceptanceThreshold();
                const double linf_tol = sim.verificationCase()->linfAcceptanceThreshold();

                const bool accuracyPassed = norms.l2_rms <= l2_tol && norms.linf <= linf_tol;

                finestSummary.caseName = caseName;
                finestSummary.solver = solver::to_string(levelCfg.solver.method);
                finestSummary.meshType = meshType;
                finestSummary.meshSize = meshSize;
                finestSummary.gradient = gradientToString(levelCfg.discretization.gradientScheme);
                finestSummary.reconstruction = reconstructionToString(levelCfg.discretization.reconstructionScheme);

                finestSummary.l2Error = norms.l2_rms;
                finestSummary.linfError = norms.linf;
                finestSummary.qoiValue = qoiValue;

                finestSummary.l2AcceptanceTol = l2_tol;
                finestSummary.linfAcceptanceTol = linf_tol;

                finestSummary.accuracyPassed = accuracyPassed;

                finestSummary.refinementEnabled = refinementEnabled;
                finestSummary.refinementPassed = false;
                finestSummary.l2Order = 0.0;
                finestSummary.linfOrder = 0.0;
            }
        }

        if (refinementEnabled)
        {
            GridConvergenceStudy study = VerificationAnalyzer::analyzeRefinement(refinementLevels);

            constexpr double orderTolerance = 0.1;

            const double l2Order = study.l2Regression.slope;
            const double linfOrder = study.linfRegression.slope;
            const double qoiOrder = study.qoiRegression.slope;
            const double gciTolerance = verificationCase.acceptance.qoiGciTolerance;

            const auto& coarse = refinementLevels[refinementLevels.size() - 2];
            const auto& fine   = refinementLevels[refinementLevels.size() - 1];

            constexpr double safetyFactor = 1.25;

            const double refinementRatio = coarse.h / fine.h;

            const double l2Richardson = VerificationAnalyzer::richardsonExtrapolation(
                                        coarse.l2,
                                        fine.l2,
                                        refinementRatio,
                                        l2Order);
            const double linfRichardson = VerificationAnalyzer::richardsonExtrapolation(
                                        coarse.linf,
                                        fine.linf,
                                        refinementRatio,
                                        linfOrder);
            const double qoiRichardson = VerificationAnalyzer::richardsonExtrapolation(
                                        coarse.qoiValue,
                                        fine.qoiValue,
                                        refinementRatio,
                                        qoiOrder);

            const auto l2GCI = VerificationAnalyzer::gridConvergenceIndex(
                                        coarse.l2,
                                        fine.l2,
                                        refinementRatio,
                                        l2Order,
                                        safetyFactor);

            const auto linfGCI = VerificationAnalyzer::gridConvergenceIndex(
                                        coarse.linf,
                                        fine.linf,
                                        refinementRatio,
                                        linfOrder,
                                        safetyFactor);

            const auto qoiGCI = VerificationAnalyzer::gridConvergenceIndex(
                                        coarse.qoiValue,
                                        fine.qoiValue,
                                        refinementRatio,
                                        qoiOrder,
                                        safetyFactor);
                                        
            const bool refinementPassed = (std::abs(l2Order - expectedOrder) <= orderTolerance) 
                                       && (std::abs(linfOrder - expectedOrder) <= orderTolerance);

            const bool gciPassed = std::abs(qoiGCI.relativeGCI) <= gciTolerance;
        
            finestSummary.refinementPassed = refinementPassed;

            finestSummary.l2Order = l2Order;
            finestSummary.linfOrder = linfOrder;
            finestSummary.qoiOrder = qoiOrder;

            finestSummary.l2Richardson = l2Richardson;
            finestSummary.linfRichardson = linfRichardson;
            finestSummary.qoiRichardson = qoiRichardson;

            finestSummary.l2RelativeGCI = l2GCI.relativeGCI;
            finestSummary.linfRelativeGCI = linfGCI.relativeGCI;
            finestSummary.qoiRelativeGCI = qoiGCI.relativeGCI;

            finestSummary.l2AbsoluteGCI = l2GCI.absoluteGCI;
            finestSummary.linfAbsoluteGCI = linfGCI.absoluteGCI;
            finestSummary.qoiAbsoluteGCI = qoiGCI.absoluteGCI;

            finestSummary.qoiGciAcceptanceTol = gciTolerance;
            finestSummary.qoiGciPassed = gciPassed;

            finestSummary.refinementRatio = refinementRatio;
            finestSummary.safetyFactor = safetyFactor;

            double richardsonGciDifference = abs(finestSummary.qoiRichardson - finestSummary.qoiAbsoluteGCI);
            double gciExactDifference = abs(finestSummary.qoiAbsoluteGCI - exactQoi); 

            std::cout << "\n================ REFINEMENT STUDY ================\n"
                      << "Case                         : " << caseName << "\n"
                      << "Observed L2 Order            : " << l2Order << "\n"
                      << "Observed Linf Order          : " << linfOrder << "\n"
                      << "Expected Order               : " << expectedOrder << "\n"
                      << "Status                       : " << (refinementPassed ? "PASS" : "FAIL") << "\n"
                      << "Observed QoI Order           : " << qoiOrder << "\n"
                      << "Richardson QOI               : " << qoiRichardson << "\n"
                      << "|Richardson QoI - exact QoI| : " << richardsonGciDifference << "\n"
                      << "|QoI Fine - exact QoI|       : " << gciExactDifference << "\n"
                      << "GCI Comparison               : " << (gciPassed ? "PASS" : "FAIL") << "\n"
                      << "==================================================\n";
        }
    
        const auto jsonPath = caseOutputDir / (caseName + ".json");

        VerificationIO::writeReport( finestSummary, jsonPath);

        std::cout << "JSON Output: " << jsonPath << "\n";

        summary.push_back(finestSummary);
    }

    std::size_t casesPassed = 0;

    for (const auto& s : summary)
    {
        if (s.passed()) { ++casesPassed; }
    }

    const bool verificationPassed = (casesPassed == summary.size()); 

    std::cout << "\n================================================================================\n";
    std::cout << "Verification Summary\n";
    std::cout << "================================================================================\n\n";

    std::cout << std::left
              << std::setw(28) << "Case"
              << std::setw(10) << "Solver"
              << std::setw(18) << "Reconstruction"
              << std::setw(16) << "Gradient"
              << std::setw(12) << "Mesh"
              << std::setw(12) << "L2 Error"
              << std::setw(12) << "L2 Tol"
              << std::setw(16) << "Max Error/Tol"
              << std::setw(14) << "Convergence"
              << std::setw(12) << "Result"
              << "\n";

    std::cout << std::string(144, '-') << "\n";

    std::cout << std::scientific << std::setprecision(3);

    for (const auto& s : summary)
    {
        const double l2Ratio   = s.l2Error / s.l2AcceptanceTol;
        const double linfRatio = s.linfError / s.linfAcceptanceTol;

        const double maxErrorRatio = std::max(l2Ratio, linfRatio);

        std::cout << std::left
                  << std::setw(28) << s.caseName
                  << std::setw(10) << s.solver
                  << std::setw(18) << s.reconstruction
                  << std::setw(16) << s.gradient
                  << std::setw(12) << s.meshSize
                  << std::setw(12) << s.l2Error
                  << std::setw(12) << s.l2AcceptanceTol
                  << std::setw(16) << maxErrorRatio;

        if (s.refinementEnabled) { std::cout << std::setw(14) << (s.refinementPassed ? "PASS" : "FAIL"); }
        else { std::cout << std::setw(14) << "--"; }

        std::cout << std::setw(12) << (verificationPassed ? "PASS" : "FAIL");
        std::cout << "\n";
    }
    
    std::cout << "\n================ FINAL RESULT ================\n";
    std::cout << "Verification Cases Passed: "
              << casesPassed << "/" << summary.size()
              << "\n";

    std::cout << "Overall Verification: "
              << (verificationPassed ? "PASS" : "FAIL")
              << "\n";

    return verificationPassed;
}
