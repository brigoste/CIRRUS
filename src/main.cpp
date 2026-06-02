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

#include <locale>   // for setlocale
#include <codecvt>  // for UTF-8 conversion (C++11/14/17)
#include <algorithm>
#include <cmath>


// TODO:
//      Get our Python Plotting to work

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

        // double sumA = 0.0;
        // for (std::size_t i = 0; i < sim.system().size(); ++i)
        // {
        //     for (const auto& [j, aij] : sim.system().row(i))
        //         sumA += std::abs(aij);
        // }

        // std::cout << "[DEBUG] matrix L1 norm = " << sumA << "\n";

        std::cout << "# of cells = " << sim.mesh().ncells()
            << "\n# of faces = " << sim.mesh().nfaces() << "\n";

        auto phi = sim.solve();

        std::cout << "\n================ SOLVER COMPLETE ================\n\n";

        auto minIt = std::min_element(phi.begin(), phi.end());
        auto maxIt = std::max_element(phi.begin(), phi.end());

        std::cout << "Min Value: " << *minIt << "\n";
        std::cout << "Max Value: " << *maxIt << "\n";

        auto r = computeResidual(sim.system(), phi);

        double maxAbsResidual = 0.0;

        for (double ri : r)
        {
            maxAbsResidual = std::max(maxAbsResidual, std::abs(ri));
        }

        std::cout << "Max |Residual|: "
                << maxAbsResidual
                << "\n";

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
