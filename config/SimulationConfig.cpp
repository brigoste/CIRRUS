#include "config/SimulationConfig.hpp"
#include "Solver/SolverMethod.hpp"

SimulationConfig defaultConfig()
{
    SimulationConfig cfg;

    // -------------------------
    // Mesh
    // -------------------------
    cfg.mesh.type = "uniform";
    cfg.mesh.n = 80;
    cfg.mesh.L = 0.5;

    // -------------------------
    // Physics
    // -------------------------
    cfg.physics.k = 100.0;
    cfg.physics.A = 0.01;

    // -------------------------
    // Source terms
    // -------------------------
    cfg.source.Su = 1000.0;
    cfg.source.Sp = 0.0;

    // -------------------------
    // Solver
    // -------------------------
    cfg.solver.type = SolverMethod::TDMA;
    cfg.solver.tol = 1e-8;
    cfg.solver.max_iter = 10000;
    cfg.solver.omega = 1.2;

    // -------------------------
    // Boundary conditions (test case only)
    // -------------------------
    cfg.bcs = {
        BoundaryConfig{
            .face = BoundaryFace::Left,
            .type = BCType::Dirichlet,
            .value = 300.0
        },
        BoundaryConfig{
            .face = BoundaryFace::Right,
            .type = BCType::Neumann,
            .flux = 0.0
        }
    };

    return cfg;
}
