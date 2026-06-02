#include "Simulation.hpp"

#include "mesh/Mesh1D.hpp"
// #include "mesh/Mesh2D.hpp"
#include "discretization/FiniteVolumeOperator.hpp"
#include "discretization/FluxBuilder.hpp"

#include <stdexcept>

// ============================================================
// Constructor
// ============================================================

Simulation::Simulation(const SimulationConfig& cfg)
    : cfg_(cfg)
{
    // std::cout << "ctor: entered\n";
    std::cout
        << "\n================ SOLVER ================\n\n"
        << "Method: "
        << solver::to_string(cfg_.solver.method)
        << "\n\n";

    if (cfg.mesh.type == "1D")
    {
        // std::cout << "ctor: creating mesh\n";
        mesh_ = std::make_unique<Mesh1D>(cfg.mesh.n, cfg.mesh.L);
        // std::cout << "ctor: mesh created\n";
    }
    else
    {
        throw std::runtime_error("Unsupported mesh");
    }

    // std::cout << "ctor: resizing system\n";
    sys_.resize(mesh_->ncells());
    // std::cout << "ctor: system resized\n";

    // std::cout << "ctor: creating flux\n";
    flux_ = std::make_unique<FluxAccumulator>(mesh_->ncells());
    // std::cout << "ctor: flux created\n";

    // std::cout << "ctor: binding BCs\n";
    bindBoundaryConditions(cfg);
    // std::cout << "ctor: BCs bound\n";

    // std::cout << "ctor: finished\n";
}

// ============================================================
// Assembly
// ============================================================

void Simulation::assemble()
{
    // std::cout << ">>> REAL Simulation::assemble() CALLED <<<\n";
    // std::cout << "[Simulation] assemble start\n";

    const HeatEquationModel model{
        cfg_.physics.k,
        false,
        cfg_.source.Su,
        cfg_.source.Sp
    };

    const MeshBase& mesh = *mesh_;

    FluxBuilder::buildFlux(mesh, model, boundary_, *flux_);
    // std::cout << "[Simulation] after buildFlux\n";

    FiniteVolumeOperator::assemble(
        // mesh,
        // model,
        *flux_,
        sys_);

    // std::cout << "[Simulation] after FV assemble\n";
}

// ============================================================
// Solve (external responsibility)
// ============================================================

std::vector<double> Simulation::solve()
{
    const auto& solverCfg = cfg_.solver;

    switch (solverCfg.method)
    {
        case solver::Method::CG:
            return CG(sys_, solverCfg.max_iter, solverCfg.tol);

        case solver::Method::GS:
            return GaussSeidel(sys_, solverCfg.max_iter, solverCfg.tol);

        case solver::Method::SOR:
            return SOR(sys_, solverCfg.max_iter, solverCfg.tol, solverCfg.omega);

        case solver::Method::TDMA:
            return TDMA(sys_);

        default:
            throw std::runtime_error("Unknown solver method");
    }
}

// ============================================================
// Boundary binding
// ============================================================

void Simulation::bindBoundaryConditions(const SimulationConfig& cfg)
{
    boundary_ = BoundaryPatchSystem();

    const std::size_t nfaces = mesh_->nfaces();

    for (const auto& bc : cfg.boundary)
    {
        if (bc.faceIndex >= nfaces)
            throw std::runtime_error(
                "Invalid BC faceIndex: " + std::to_string(bc.faceIndex));

        boundary_.set(bc.faceIndex, bc.condition);
    }
}

// ============================================================
// Optional helper
// ============================================================

std::size_t Simulation::resolveBoundaryFace(const std::string& loc)
{
    if (loc == "left")  return 0;
    if (loc == "right") return mesh_->nfaces() - 1;

    throw std::runtime_error("Unknown boundary location: " + loc);
}
