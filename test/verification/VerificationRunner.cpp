#include "tests/verification/VerificationRunner.hpp"
#include "mesh/MeshBase.hpp"
#include "config/SimulationConfig.hpp"
#include "simulation/Simulation.hpp"
#include <memory>
#include <stdexcept>
#include <iostream>
#include <cmath>

void VerificationRunner::run(const SimulationConfig& cfg)
{
    if (!cfg.verification.enabled)
        return;

    for (const auto& caseName : cfg.verification.cases)
    {
        std::cout
            << "\n=================================\n"
            << "Running verification case: "
            << caseName
            << "\n=================================\n";

        auto casePtr =
            VerificationCaseFactory::create(caseName);

        // SimulationConfig caseCfg = cfg;
        SimulationConfig caseCfg = casePtr->config();
        Simulation sim(caseCfg);
        // Simulation sim(cfg);

        sim.setVerificationCase(
            std::move(casePtr));

        sim.assemble();

        auto phi = sim.solve(); 
        const auto& outputDir = cfg.verification.output.directory;
        const std::string csvFile = outputDir + "/" + caseName + ".csv";
        const std::string summaryFile = outputDir + "/" + caseName + ".json";

        std::cout << "CSV Output     : " << csvFile << "\n" 
                  << "Summary Output : " << summaryFile << "\n";

        double l2 = 0.0;
        double linf = 0.0;

        computeNorms(
            sim.mesh(),
            phi,
            *sim.verificationCase(),
            l2,
            linf);

        std::cout
            << "\n================ VERIFICATION ================\n"
            << "Case      : " << caseName << "\n"
            << "L2 Norm   : " << l2 << "\n"
            << "Linf Norm : " << linf << "\n"
            << "=============================================\n";
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
