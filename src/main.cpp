#include <iostream>
#include <memory>
#include <filesystem>

#include "io/FieldWriter.hpp"
#include "io/VTKWriter.hpp"

#include "mesh/Mesh1D.hpp"
#include "Solver/SolverMethod.hpp"

// HT SOLVERS
#include "system/HeatCase1D.hpp"
#include "system/HeatSystem1D.hpp"
// #include "system/HeatCase2D.hpp" 
// #include "system/HeatSystem2D.hpp"
// #include "system/HeatCase3D.hpp"
// #include "system/HeatSystem3D.hpp"

// FLUID SOLVERS - need to implment
// #include "system/FluidCase1D.hpp"        
// #include "system/FluidSystem1D.hpp"
// #include "system/FluidCase2D.hpp"
// #include "system/FluidSystem2D.hpp"
// #include "system/FluidCase3D.hpp"
// #include "system/FluidSystem3D.hpp"

#include "linear_system/Residual.hpp"

// JSON includes
#include "config/SimulationConfigLoader.hpp"
#include "config/SimulationConfig.hpp"


int main()
{
    std::cout << "\n================ INITIALIZING SYSTEM ================\n";

    bool useJSON = false;

    SimulationConfig cfg = defaultConfig(); 

    if (useJSON)
    {
        std::cout << "Loading JSON config...\n";
        cfg = loadConfig("C:/Users/E40112856/Packages/CIRRUS/output/metadata.json");
    }
    else
    {
        std::cout << "Using default config...\n";
    }

    // -----------------------------
    // Build physics from config
    // -----------------------------
    HeatCase1D physics;

    physics.n = cfg.mesh.n;
    physics.L = cfg.mesh.L;

    physics.k = cfg.physics.k;
    physics.A = cfg.physics.A;

    physics.Su = [Su0 = cfg.source.Su](const Point&) { return Su0; };
    physics.Sp = [Sp0 = cfg.source.Sp](const Point&) { return Sp0; };

    // physics.bcs = buildBoundaryContexts(cfg.bcs, cfg.mesh);

    // -----------------------------
    // Mesh + system
    // -----------------------------
    Mesh1D mesh(cfg.mesh.n, cfg.mesh.L);
    HeatSystem1D system(mesh, physics);

    // -----------------------------
    // Solve
    // -----------------------------
    system.assemble();

    auto phi = system.solve(
        cfg.solver.type,
        cfg.solver.max_iter,
        cfg.solver.tol,
        cfg.solver.omega
    );

    std::cout << "\n================ SOLVING COMPLETE ================\n";

    auto r = computeResidual(system.system(), phi);

    VTKWriter::writeVTU(mesh, phi, "../output/solution.vtu");

    FieldWriter::writeCSVDebug(
        mesh,
        phi,
        system.system().rhs(),
        r,
        "../output/solution.csv"
    );

    return 0;
}
