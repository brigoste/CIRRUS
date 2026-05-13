#include <iostream>
#include <memory>

#include "system/HeatCase1D.hpp"
#include "system/HeatSystem1D.hpp"
#include "mesh/Mesh1D.hpp"
#include "Solver/SolverMethod.hpp"
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

    cfg.method = SolverMethod::CG;
    cfg.iter   = 5000;
    cfg.tol    = 1e-8;
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
    auto T = system.solve(
        cfg.method,
        cfg.iter,
        cfg.tol,
        cfg.omega
    );

    std::cout << "Solution computed.\n";

    return 0;
}
