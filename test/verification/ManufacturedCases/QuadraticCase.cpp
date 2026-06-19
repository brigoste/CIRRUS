#include "tests/verification/ManufacturedCases/Quadratic1D.hpp"

// #include "tests/verification/VerificationAutoRegister.hpp"
// #include "tests/verification/VerificationRegistry.hpp"

double Quadratic1D::exact(
    double x,
    double) const
{
    return x * x;
}

double Quadratic1D::laplacian(
    double,
    double) const
{
    return 2.0;      // -2*k
}

SimulationConfig Quadratic1D::config() const
{
    SimulationConfig cfg;

    // -------------------------
    // Mesh
    // -------------------------
    cfg.mesh.type = "line1D";
    cfg.mesh.nx = 320;
    cfg.mesh.lx = 1.5;

    // -------------------------
    // Physics
    // -------------------------
    cfg.physics.type = "heat";
    cfg.physics.k = 100.0;
    cfg.physics.Su = 0.0;
    cfg.physics.Sp = 0.0;

    // -------------------------
    // Solver
    // -------------------------
    cfg.solver.method = solver::Method::TDMA;
    cfg.solver.tol = 1e-12;
    cfg.solver.max_iter = 5000;
    cfg.solver.omega = 1.0;

    // -------------------------
    // Boundary conditions
    // -------------------------
    cfg.boundary.clear();

    BoundaryConfig leftBC;
    leftBC.group = 0;
    leftBC.condition.type = bc::Type::Dirichlet;
    leftBC.condition.value = 0.0;   // exact(0) = 0

    BoundaryConfig rightBC;
    rightBC.group = 1;
    rightBC.condition.type = bc::Type::Dirichlet;
    rightBC.condition.value = 2.25;  // exact(1.5) = 2.25

    cfg.boundary.push_back(leftBC);
    cfg.boundary.push_back(rightBC);

    return cfg;
}
