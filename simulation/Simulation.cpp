#include "Simulation.hpp"

#include "mesh/Mesh1D.hpp"
#include "mesh/QuadMesh2D.hpp"
#include "discretization/FiniteVolumeOperator.hpp"
#include "discretization/FluxBuilder.hpp"
#include "discretization/operators/GradientFactory.hpp"
#include "test/verification/VerificationCaseFactory.hpp"

#include "solver/preconditioners/PreconditionerFactory.hpp"

#include "utils/Timer.hpp" // Runtime optimziation checking

#include <stdexcept>

// ============================================================
// Field initializer
// ============================================================

void Simulation::initializeFields()
{
    fields_.createScalar(
        physics_->solutionField(),
        *mesh_,
        FieldLocation::Cell,
        physics_->initialSolutionValue()
    );
}
// ============================================================
// Constructor
// ============================================================

Simulation::Simulation(const SimulationConfig& cfg)
: cfg_(cfg),
  assembled_(false)
{
    // 1. Physics
    physics_ = PhysicsFactory::create(cfg.physics);

    // 2. Gradient
    gradient_ =
        std::make_unique<GradientOperator>(
            GradientFactory::create(
                cfg.discretization.gradientScheme
            )
        );

    // -------------------------
    // 3. Mesh
    // -------------------------
    if (cfg.mesh.type == "line1D")
    {
        mesh_ = std::make_unique<Mesh1D>(
            cfg.mesh.nx,
            cfg.mesh.lx
        );
    }
    else if (cfg.mesh.type == "quad2D")
    {
        mesh_ = std::make_unique<QuadMesh2D>(
            cfg.mesh.nx,
            cfg.mesh.ny,
            cfg.mesh.lx,
            cfg.mesh.ly
        );
    }
    else
    {
        throw std::runtime_error(
            "Unsupported mesh type: " + cfg.mesh.type
        );
    }

    // -------------------------
    // 4. Create fields
    // -------------------------
    initializeFields();


    // -------------------------
    // 5. Allocate solver data
    // -------------------------
    flux_ = std::make_unique<FluxAccumulator>(
        mesh_->ncells()
    );

    sys_.resize(mesh_->ncells());

    fvOperator_ =
        std::make_unique<FiniteVolumeOperator>(
            convectionScheme_
        );


    // -------------------------
    // 6. Boundary conditions
    // -------------------------
    bindBoundaryConditions(cfg);


    std::cout << "NCELLS = " << mesh_->ncells() << "\n";
    std::cout << "NFACE  = " << mesh_->nfaces() << "\n";
    std::cout << "NX     = " << cfg.mesh.nx << "\n";
    std::cout << "NY     = " << cfg.mesh.ny << "\n";
}

void Simulation::setVerificationCase(std::unique_ptr<VerificationCase> verificationCase) 
{ 
    verificationCase_ = std::move(verificationCase); 
}

// ============================================================
// Assemble (external responsibility)
// ============================================================

void Simulation::assemble()
{
    flux_->reset();
    sys_.clear();

    FluxBuilder fluxBuilder;

    fluxBuilder.buildFlux( *mesh_, *physics_, boundary_, *flux_, verificationCase_.get()); 

    fvOperator_->assemble( *flux_, sys_);

    assembled_ = true;
}

// ============================================================
// Solve (external responsibility)
// ============================================================

void Simulation::solve()
{
    const auto& solverCfg = cfg_.solver;

    if (!assembled_) 
    { 
        throw std::runtime_error("System not assembled"); 
    }

    std::cout << "System Type: " << physics::to_string(cfg_.physics.type) << "\n";
    std::cout << "Solver: " << solver::to_string(cfg_.solver.method) << "\n";

    std::vector<double> phi;

    switch (solverCfg.method)
    {
        case solver::Method::BiCGSTAB:
        {
            auto M = createPreconditioner( solverCfg.preconditioner );
            M->setup(sys_);
            std::cout << "Preconditioner: " << M->name() << "\n";
            phi = BiCGSTAB(sys_,solverCfg.max_iter,solverCfg.tol,*M);
            break;
        }

        case solver::Method::CG:
        {
            auto M = createPreconditioner( solverCfg.preconditioner );
            M->setup(sys_);
            std::cout << "Preconditioner: " << M->name() << "\n";
            phi = CG( sys_, solverCfg.max_iter, solverCfg.tol, *M );
            break;
        }

        case solver::Method::GS:
            phi = GaussSeidel( sys_, solverCfg.max_iter, solverCfg.tol );
            break;

        case solver::Method::SOR:
            phi = SOR( sys_, solverCfg.max_iter, solverCfg.tol, solverCfg.omega );
            break;

        case solver::Method::TDMA:
            phi = TDMA(sys_);
            break;

        default:
            throw std::runtime_error( "Solver method not part of directory." );
    }

    // ---------------------------------------
    // Transfer solution into field storage
    // ---------------------------------------
    auto& solutionField = fields_.scalar(physics_->solutionField());

    if (solutionField.size() != phi.size())
    {
        throw std::runtime_error( "Solution size does not match solution field size." );
    }

    for (std::size_t i = 0; i < phi.size(); ++i) { solutionField[i] = phi[i]; }
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
