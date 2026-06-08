#include "Simulation.hpp"

#include "mesh/Mesh1D.hpp"
#include "mesh/QuadMesh2D.hpp"
#include "discretization/FiniteVolumeOperator.hpp"
#include "discretization/FluxBuilder.hpp"

#include <stdexcept>

// ============================================================
// Constructor
// ============================================================

Simulation::Simulation(const SimulationConfig& cfg)
    : cfg_(cfg),
      assembled_(false)
{
    // -------------------------
    // 1. Build physics model FIRST
    // -------------------------
    model_ = PhysicsFactory::create(cfg.physics);

    // -------------------------
    // 2. Mesh
    // -------------------------
    if (cfg.mesh.type == "line1D")
    {
        mesh_ = std::make_unique<Mesh1D>(cfg.mesh.nx, cfg.mesh.lx);
    }
    else if (cfg.mesh.type == "quad2D")
    {
        mesh_ = std::make_unique<QuadMesh2D>(cfg.mesh.nx, cfg.mesh.ny, cfg.mesh.lx, cfg.mesh.ly);
    }
    else
    {
        std::cout << "Mesh type declared: " << cfg.mesh.type << "\n";
        throw std::runtime_error("Unsupported mesh");
    }

    if (cfg.verification.enabled)
    {
        verificationCase_ = verificationRegistry_.instance().create(cfg.verification);
    }

    // -------------------------
    // 3. Allocate solver data
    // -------------------------
    flux_ = std::make_unique<FluxAccumulator>(mesh_->ncells());
    sys_.resize(mesh_->ncells());

    // -------------------------
    // 4. Boundary conditions
    // -------------------------
    bindBoundaryConditions(cfg);
}

// ============================================================
// Assemble (external responsibility)
// ============================================================


void Simulation::assemble()
{
    flux_->reset();
    sys_.clear();

    if (verificationCase_)
    {
        FluxBuilder::buildFlux(
            *mesh_,
            *model_,
            boundary_,
            *flux_,
            verificationCase_.get());
    }
    else
    {
        FluxBuilder::buildFlux(
            *mesh_,
            *model_,
            boundary_,
            *flux_,
            nullptr);
    }

    FiniteVolumeOperator::assemble(
        *flux_,
        sys_);

    assembled_ = true;
}

// ============================================================
// Solve (external responsibility)
// ============================================================

std::vector<double> Simulation::solve()
{
    const auto& solverCfg = cfg_.solver;
    if (!assembled_)
        throw std::runtime_error("System not assembled");

    std::cout << "System Type: " << cfg_.physics.type << "\n";
    std::cout << "Solver: " << to_string(cfg_.solver.method) << "\n";

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

    for (const auto& bc : cfg.boundary)
    {
        boundary_.setGroup(bc.group, bc.condition);
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
