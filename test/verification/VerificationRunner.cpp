#include "tests/verification/VerificationRunner.hpp"

#include "config/SimulationConfig.hpp"
#include "config/PathUtils.hpp"
#include "config/PathContext.hpp"

#include "simulation/Simulation.hpp"
#include "mesh/MeshBase.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"

#include "tests/verification/VerificationCaseFactory.hpp"
#include "tests/verification/VerificationIO.hpp"
#include "tests/verification/ErrorMetrics.hpp"

#include "io/PlotUtils.hpp"

#include <iostream>
#include <filesystem>
#include <stdexcept>

void VerificationRunner::run(
    const SimulationConfig& cfg,
    const PathContext& paths)
{
    if (!cfg.verificationSuite.enabled)
        return;

    for (const auto& caseName : cfg.verificationSuite.cases)
    {
        std::cout
            << "\n=================================\n"
            << "Running verification case: " << caseName
            << "\n=================================\n";

        auto casePtr = VerificationCaseFactory::create(caseName);

        SimulationConfig caseCfg = casePtr->config();
        Simulation sim(caseCfg);

        sim.setVerificationCase(std::move(casePtr));

        sim.assemble();
        auto phi = sim.solve();

        const MeshBase& mesh = sim.mesh();
        const auto& verifCase = *sim.verificationCase();

        PointField field =
            BoundaryReconstructor::reconstruct(
                mesh,
                sim.boundary(),
                sim.model(),
                phi);

        std::vector<double> residual(phi.size(), 0.0);

        // -----------------------------
        // exact field for norms
        // -----------------------------
        std::vector<double> exactField(mesh.ncells());

        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            const auto& xc = mesh.cellCenter(c);
            exactField[c] = verifCase.exact(xc.x[0], xc.x[1]);
        }

        auto norms = ErrorNorms::compute(
            mesh,
            phi,
            exactField);

        // -----------------------------
        // PATHS (NOW PURE PathContext)
        // -----------------------------
        // auto csvPath  = paths.verificationCSV(caseName);
        // auto jsonPath = paths.verificationJSON(caseName);
        auto csvPath  = paths.outputRoot / "solution.csv";
        auto jsonPath = paths.outputRoot / "solution.json";
        auto vtkPath  = paths.outputRoot / "solution.vtu";

        std::filesystem::create_directories(csvPath.parent_path());

        // -----------------------------
        // IO
        // -----------------------------
        VerificationIO::writeCSV(
            sim,
            phi,
            csvPath);

        VerificationIO::writeSummary(
            caseName,
            norms.l2_energy,
            norms.linf,
            jsonPath);

        // -----------------------------
        // output
        // -----------------------------
        std::cout
            << "\n================ VERIFICATION ================\n"
            << "Case      : " << caseName << "\n"
            << "L2 Norm   : " << norms.l2_energy << "\n"
            << "Linf Norm : " << norms.linf << "\n"
            << "CSV Output: " << csvPath << "\n"
            << "JSON Output: " << jsonPath << "\n"
            << "=============================================\n";

        if (cfg.verificationSuite.plot_enabled)
        {
            std::cout << "Plotting...\n";
            runPlot(csvPath.generic_string());
        }
    }
}
