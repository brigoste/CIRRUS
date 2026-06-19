#include "tests/verification/ManufacturedCases/Sinusoidal2D.hpp"

// #include "tests/verification/VerificationAutoRegister.hpp"
// #include "tests/verification/VerificationRegistry.hpp"
#include "utils/MathConstants.hpp"

SimulationConfig Sinusoidal2D::config() const
{
    SimulationConfig cfg;

    // -------------------------
    // Mesh (IMPORTANT: 2D now)
    // -------------------------
    cfg.mesh.type = "quad2D";
    cfg.mesh.nx = 160;
    cfg.mesh.ny = 160;
    cfg.mesh.lx = 1.0;
    cfg.mesh.ly = 1.0;

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
    cfg.solver.method = solver::Method::CG; // or CG/SOR if you later extend to 2D coupling
    cfg.solver.tol = 1e-12;
    cfg.solver.max_iter = 20000;
    cfg.solver.omega = 1.2;

    // -------------------------
    // Boundary conditions
    // -------------------------
    cfg.boundary.clear();

    // Dirichlet everywhere: phi = sin(pi x) sin(pi y)
    // so boundary is zero on all edges

    auto makeBC = [](int g)
    {
        BoundaryConfig bc;
        bc.group = g;
        bc.condition.type = bc::Type::Dirichlet;
        bc.condition.value = 0.0;
        return bc;
    };

    cfg.boundary.push_back(makeBC(0));
    cfg.boundary.push_back(makeBC(1));
    cfg.boundary.push_back(makeBC(2));
    cfg.boundary.push_back(makeBC(3));

    return cfg;
}

double Sinusoidal2D::exact(
    double x,
    double y) const
{
    return std::sin(math::PI * x) *
           std::sin(math::PI * y);
}

double Sinusoidal2D::laplacian(
    double x,
    double y) const
{
    return -2.0 * math::PI * math::PI *
           std::sin(math::PI * x) *
           std::sin(math::PI * y);
}
