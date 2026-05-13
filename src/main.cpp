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

#include "system/SimulationConfig.hpp"

int main()
{
    std::cout << "\n================ INITIALIZING SYSTEM ================\n";

    // -----------------------------
    // 1. Physics definition
    // -----------------------------
    HeatCase1D physics;

    // Define Geometry and material
    physics.n = 80;
    physics.L = 0.5;
    physics.A = 0.01;
    physics.k = 100.0;

    // Define Source Terms
    physics.Su = 1000.0;
    physics.Sp = 0.0;

    // Apply Boundary Conditions
    physics.bcs.push_back(
    BoundaryConditionDescriptor::Dirichlet(BoundaryFace::Left, 300.0)
    );

    physics.bcs.push_back(
        BoundaryConditionDescriptor::Neumann(BoundaryFace::Right, 0.0)
    );

    SimulationConfig cfg;

    cfg.method = SolverMethod::TDMA;
    cfg.iter   = 10000;
    cfg.tol    = 1e-5;
    cfg.omega  = 1.2;

    // -----------------------------
    // 2. Mesh
    // -----------------------------
    Mesh1D mesh(physics.n, physics.L);

    // -----------------------------
    // 3. System
    // -----------------------------
    HeatSystem1D system(mesh, physics);

    // -----------------------------
    // 4. Assembly
    // -----------------------------
    system.assemble();

    std::cout << "\n================ SOLVING ================\n";

    // -----------------------------
    // 5. Solve
    // -----------------------------
    auto phi = system.solve(
        cfg.method,     // solver method
        cfg.iter,       // n-iterations
        cfg.tol,        // convergence tolerance
        cfg.omega       // relaxation factor
    );

    std::cout << "\nSolution computed.\n";

    std::cout << "\n============================================\n";

    std::filesystem::path out = "../output/solution.vtu";
    VTKWriter::writeVTU(mesh, phi, out);

    std::cout << "\nSolution written to VTK\n\n";

    return 0;
}
