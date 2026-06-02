#include <iostream>
#include <memory>
#include <filesystem>

#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"

#include "mesh/Mesh1D.hpp"
#include "Solver/SolverMethod.hpp"

// HT SOLVERS
#include "system/HeatSystem1D.hpp"
// #include "system/HeatSystem2D.hpp"
// #include "system/HeatSystem3D.hpp"

// FLUID SOLVERS - need to implment
// #include "system/FluidSystem1D.hpp"
// #include "system/FluidSystem2D.hpp"
// #include "system/FluidSystem3D.hpp"

#include "linear_system/Residual.hpp"

#include "simulation/Simulation.hpp"

// JSON includes
// #include "config/SimulationConfigLoader.hpp"
#include "config/SimulationConfig.hpp"


// TODO:
//      Get our Python Plotting working
//      We have it working on the other laptop, but I can't get pandas on this one
//      As such, we just need to get the code to github after we copy over the python
//      scripting.

int main()
{
    try
    {
        std::cout.flush();
        std::cout << "\n\n================ INITIALIZING SYSTEM ================\n\n";

        bool useJSON = true;

        SimulationConfig cfg;

        if (useJSON)
        {
            std::cout << "Loading JSON config...\n";
            cfg = loadConfig("C:/Users/E40112856/Packages/CIRRUS/output/metadata.json");
        }
        else
        {
            std::cout << "Using default config...\n";
            cfg = defaultConfig();

            // -----------------------------
            // Source terms (constant example)
            // -----------------------------
            cfg.source.Su = [](const Point&) { return 1000.0; };
            cfg.source.Sp = [](const Point&) { return 0.0; };
        }

        // std::cout << "BEFORE SIM\n";

        Simulation sim(cfg);

        // std::cout << "AFTER SIM\n";

        sim.assemble();

        // std::cout << "AFTER ASSEMBLE\n";

        double sumA = 0.0;
        for (std::size_t i = 0; i < sim.system().size(); ++i)
        {
            for (const auto& [j, aij] : sim.system().row(i))
                sumA += std::abs(aij);
        }

        // std::cout << "[DEBUG] matrix L1 norm = " << sumA << "\n";

        std::cout << "ncells=" << sim.mesh().ncells()
            << "\nnfaces=" << sim.mesh().nfaces() << "\n";

        std::cout << "Solver: " << solver::to_string(cfg.solver.method);

        auto phi = sim.solve();

        std::cout << "\n\n================ SOLVER COMPLETE ================\n\n";

        auto r = computeResidual(sim.system(), phi);

        VTKWriter::writeVTU(
            sim.mesh(),
            phi,
            "../output/solution.vtu"
        );

        FieldWriter::writeCSVDebug(
            sim.mesh(),
            phi,
            sim.system().RHS(),
            r,
            "../output/solution.csv"
        );
    }
    catch (const std::exception& e)
    {
        std::cerr << "EXCEPTION: " << e.what() << "\n";
    }
    catch (...)
    {
        std::cerr << "UNKNOWN EXCEPTION\n";
    }

    return 0;
}
