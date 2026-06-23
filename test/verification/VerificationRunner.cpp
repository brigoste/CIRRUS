#include "tests/verification/VerificationRunner.hpp"

#include "config/SimulationConfig.hpp"
#include "config/PathUtils.hpp"

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

void VerificationRunner::run(const SimulationConfig& cfg)
{
    if (!cfg.verification.enabled)
        return;

    for (const auto& caseName : cfg.verification.cases)
    {
        std::cout
            << "\n=================================\n"
            << "Running verification case: " << caseName
            << "\n=================================\n";

        // -------------------------------------------------
        // Build case + simulation
        // -------------------------------------------------
        auto casePtr = VerificationCaseFactory::create(caseName);
        SimulationConfig caseCfg = casePtr->config();

        Simulation sim(caseCfg);
        sim.setVerificationCase(std::move(casePtr));

        // -------------------------------------------------
        // Solve
        // -------------------------------------------------
        sim.assemble();
        auto phi = sim.solve();

        const MeshBase& mesh = sim.mesh();
        const auto& verifCase = *sim.verificationCase();

        // -------------------------------------------------
        // Reconstruct field (for CSV output)
        // -------------------------------------------------
        PointField field =
            BoundaryReconstructor::reconstruct(
                mesh,
                sim.boundary(),
                sim.model(),
                phi);

        std::vector<double> residual(phi.size(), 0.0);

        // -------------------------------------------------
        // Error norms (UNIFIED)
        // -------------------------------------------------
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

        // -------------------------------------------------
        // Paths
        // -------------------------------------------------
        auto csvPath   = Paths::verificationCSV(cfg, caseName);
        auto jsonPath  = Paths::verificationJSON(cfg, caseName);

        std::filesystem::create_directories(csvPath.parent_path());

        // -------------------------------------------------
        // IO layer
        // -------------------------------------------------
        VerificationIO::writeCSV(
            sim,
            phi,
            csvPath);


        VerificationIO::writeSummary(
            caseName,
            norms.l2_energy,
            norms.linf,
            jsonPath);

        // -------------------------------------------------
        // Console output
        // -------------------------------------------------
        std::cout
            << "\n================ VERIFICATION ================\n"
            << "Case      : " << caseName << "\n"
            << "L2 Norm   : " << norms.l2_energy << "\n"
            << "Linf Norm : " << norms.linf << "\n"
            << "CSV Output: " << csvPath << "\n"
            << "JSON Output: " << jsonPath << "\n"
            << "=============================================\n";

        // -------------------------------------------------
        // Plotting
        // -------------------------------------------------
        if (cfg.verification.plot_enabled)
        {
            std::cout << "Plotting...\n";
            runPlot(csvPath.string());
        }
    }
}
