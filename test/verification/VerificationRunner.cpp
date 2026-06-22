#include "tests/verification/VerificationRunner.hpp"
#include "mesh/MeshBase.hpp"
#include "config/SimulationConfig.hpp"
#include "simulation/Simulation.hpp"
#include "io/PointField.hpp"
#include "postprocessing/BoundaryReconstructor.hpp"
#include "io/FieldWriter.hpp"
#include <memory>
#include <stdexcept>
#include <iostream>
#include <cmath>
#include <fstream>
#include "nlohmann/json.hpp"
#include "io/PlotUtils.hpp"

void VerificationRunner::run(const SimulationConfig& cfg)
{
    if (!cfg.verification.enabled)
        return;

    namespace fs = std::filesystem;

    // -------------------------------------------------
    // Output root (single source of truth)
    // -------------------------------------------------
    fs::path outputRoot = cfg.verification.output.directory;
    fs::create_directories(outputRoot);

    for (const auto& caseName : cfg.verification.cases)
    {
        std::cout << "\n=================================\n"
                  << "Running verification case: " << caseName
                  << "\n=================================\n";

        // -------------------------------------------------
        // Build and run case
        // -------------------------------------------------
        auto casePtr = VerificationCaseFactory::create(caseName);

        SimulationConfig caseCfg = casePtr->config();
        Simulation sim(caseCfg);

        sim.setVerificationCase(std::move(casePtr));

        sim.assemble();
        auto phi = sim.solve();

        // -------------------------------------------------
        // Paths (ALL filesystem-safe)
        // -------------------------------------------------
        fs::path basePath   = outputRoot / caseName;
        fs::path csvPath    = basePath;
        csvPath.replace_extension(".csv");

        fs::path jsonPath   = basePath;
        jsonPath.replace_extension(".json");

        // -------------------------------------------------
        // Compute norms
        // -------------------------------------------------
        double l2 = 0.0;
        double linf = 0.0;

        computeNorms(
            sim.mesh(),
            phi,
            *sim.verificationCase(),
            l2,
            linf);

        // -------------------------------------------------
        // Reconstruct field
        // -------------------------------------------------
        PointField field = BoundaryReconstructor::reconstruct(
            sim.mesh(),
            sim.boundary(),
            sim.model(),
            phi);

        std::vector<double> residual(phi.size(), 0.0);

        // -------------------------------------------------
        // Write CSV
        // -------------------------------------------------
        FieldWriter::writeCSVDebug(
            field,
            sim.system().RHS(),
            residual,
            csvPath.string());

        // -------------------------------------------------
        // Write JSON summary
        // -------------------------------------------------
        {
            std::ofstream json(jsonPath);
            json << "{\n"
                 << "  \"case\": \"" << caseName << "\",\n"
                 << "  \"l2\": " << l2 << ",\n"
                 << "  \"linf\": " << linf << "\n"
                 << "}\n";
        }

        // -------------------------------------------------
        // Console output
        // -------------------------------------------------
        std::cout
            << "\n================ VERIFICATION ================\n"
            << "Case      : " << caseName << "\n"
            << "L2 Norm   : " << l2 << "\n"
            << "Linf Norm : " << linf << "\n"
            << "CSV Output : " << csvPath << "\n"
            << "=============================================\n";

        // -------------------------------------------------
        // Optional plotting
        // -------------------------------------------------
        if (cfg.verification.plot_enabled)
        {
            std::cout << "Plotting:\n";
            runPlot(csvPath.string());
        }
        else
        {
            std::cout << "No visualization requested.\n";
        }
    }
}

void VerificationRunner::computeNorms(
    const MeshBase& mesh,
    const std::vector<double>& numerical,
    const VerificationCase& exact,
    double& l2,
    double& linf)
{
    const std::size_t N = mesh.ncells();

    if (numerical.size() != N)
        throw std::runtime_error("computeNorms: size mismatch");

    double sum = 0.0;
    linf = 0.0;

    for (std::size_t c = 0; c < N; ++c)
    {
        const auto& xc = mesh.cellCenter(c);

        const double x = xc.x[0];
        const double y = xc.x[1];

        const double phi_exact = exact.exact(x, y);
        const double err = numerical[c] - phi_exact;

        const double V = mesh.cellVolume(c);

        sum += err * err * V;
        linf = std::max(linf, std::abs(err));
    }

    l2 = std::sqrt(sum);
}
