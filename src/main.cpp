#include <iostream>
#include <memory>
#include <filesystem>

#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"

#include "postprocessing/BoundaryReconstructor.hpp"
#include "postprocessing/DerivedFields.hpp"
#include "postprocessing/ErrorNorms.hpp"

#include "Solver/SolverMethod.hpp"

#include "linear_system/Residual.hpp"

#include "simulation/Simulation.hpp"

// JSON includes
#include "config/SimulationConfig.hpp"
#include "tests/verification/VerificationRunner.hpp"

// Plot Setups
#include "io/PlotUtils.hpp"

#include <locale>   // for setlocale
#include <codecvt>  // for UTF-8 conversion (C++11/14/17)
#include <algorithm>
#include <cmath>

std::filesystem::path resolveOutputPath(
    const std::filesystem::path& root,
    const std::string& relative)
{
    return root / relative;
}

int main()
{
    try
    {
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        const bool useJSON = true;

        SimulationConfig cfg;

        if (useJSON)
        {
            std::cout << "Loading JSON config...\n";
            cfg = loadConfig("C:/Users/E40112856/Packages/CIRRUS/output/verification/Linear1D.json");
        }
        else
        {
            std::cout << "Using default config...\n";
            cfg = defaultConfig();
        }

        // =====================================================
        // MODE SWITCH: VERIFICATION vs NORMAL SIMULATION
        // =====================================================
        if (cfg.verification.enabled)
        {
            std::cout << "\n================ VERIFICATION MODE ================\n";

            VerificationRunner::run(cfg);

            std::cout << "\n================ VERIFICATION COMPLETE ================\n";
            return 0;
        }

        // =====================================================
        // NORMAL SIMULATION PATH
        // =====================================================
        std::cout << "\n================ USER ENCODED SIMULATION ================\n";

        Simulation sim(cfg);
        sim.assemble();

        std::cout << "# of cells = " << sim.mesh().ncells()
                  << "\n# of faces = " << sim.mesh().nfaces() << "\n";

        std::vector<double> phi = sim.solve();

        const auto& mesh = sim.mesh();
        const auto& system = sim.system();

        std::cout << "\n================ SOLVER COMPLETE ================\n\n";

        auto residual = computeResidual(system, phi);

        double maxAbsResidual = 0.0;
        for (double r : residual)
            maxAbsResidual = std::max(maxAbsResidual, std::abs(r));

        // ----------------------------
        // OUTPUT
        // ----------------------------

        // Add the boundaryies back into the solution field
        auto field = BoundaryReconstructor::reconstruct(
            mesh,
            sim.boundary(),
            sim.model(),
            phi);

        std::filesystem::path projectRoot =  std::filesystem::current_path().parent_path();

        cfg.io.output_root = (projectRoot / "output").string();

        std::cout << "Output folder: " << cfg.io.output_root << std::endl;

        std::filesystem::create_directories(cfg.io.output_root);

        std::cout << "Output directory: " << cfg.io.output_root << "\n";

        VTKWriter::writeVTU(mesh, 
                            phi, 
                            cfg.io.output_root + "/solution.vtu");

        FieldWriter::writeCSVDebug(
            field,
            system.RHS(),
            residual,
            cfg.io.output_root + "/solution.csv");

        constexpr bool plot_solution_field = true;

        std::string output_csv_filepath = cfg.io.output_root + "/solution.csv";

        if (plot_solution_field)
        {
            // runPlot(cfg.io.output_root + "/solution.csv");
            runPlot((std::filesystem::absolute(output_csv_filepath).string()));
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << "EXCEPTION: " << e.what() << "\n";
    }

    return 0;
}
