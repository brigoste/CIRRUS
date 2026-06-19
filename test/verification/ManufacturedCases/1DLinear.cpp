#include "tests/verification/ManufacturedCases/1DLinear.hpp"
// #include "tests/verification/VerificationAutoRegister.hpp"
// #include "tests/verification/VerificationRegistry.hpp"
SimulationConfig OneDLinear::config() const
{
    SimulationConfig cfg;

    // -------------------------
    // Mesh
    // -------------------------
    cfg.mesh.type = "line1D";
    cfg.mesh.nx = 20;
    cfg.mesh.lx = 1.5;

    // -------------------------
    // Physics
    // -------------------------
    cfg.physics.type = "heat";
    cfg.physics.k = 100.0;
    cfg.physics.Su = 0.0;
    cfg.physics.Sp = 0.0;

    // -------------------------
    // Solver (optional defaults)
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
    leftBC.condition.value = 300.0;

    BoundaryConfig rightBC;
    rightBC.group = 1;
    rightBC.condition.type = bc::Type::Dirichlet;
    rightBC.condition.value = 400.0;

    cfg.boundary.push_back(leftBC);
    cfg.boundary.push_back(rightBC);

    return cfg;
}

double OneDLinear::exact(double x, double) const
{
    return 300.0 + (100.0 / 1.5) * x;
}

double OneDLinear::laplacian(double,double) const
{
    return 0.0;
}
