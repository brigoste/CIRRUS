#pragma once

#include <memory>
#include <string>
#include <iostream>

#include "config/SimulationConfig.hpp"
#include "fields/FieldRegistry.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "physics/PhysicsModel.hpp"
#include "physics/PhysicsFactory.hpp"
#include "physics/ScalarTransport/ScalarTransportModel.hpp"
#include "equation_systems/LinearSystem.hpp"
#include "bc/BoundaryFace.hpp"

#include "discretization/diffusion/StandardDiffusionScheme.hpp"
#include "discretization/operators/DiffusionOperator.hpp"
#include "discretization/operators/ConvectionOperator.hpp"
#include "discretization/builders/DiffusionFluxBuilder.hpp"
#include "discretization/FiniteVolumeAssembler.hpp"
#include "discretization/FluxBuilder.hpp"
#include "discretization/gradient/GradientScheme.hpp"
#include "discretization/gradient/GradientType.hpp"
#include "discretization/gradient/GradientFactory.hpp"
#include "discretization/reconstructors/ReconstructionScheme.hpp"
#include "discretization/reconstructors/ReconstructionFactory.hpp"

#include "solver/CG.hpp"
#include "solver/SOR.hpp"
#include "solver/TDMA.hpp"
#include "solver/GaussSeidel.hpp"
#include "solver/BiCGSTAB.hpp"
#include "solver/GMRES.hpp"
#include "solver/SolverMethod.hpp"

#include "equation_systems/LinearEquationSystem.hpp"

// ============================================================
// Simulation = composition root
// ============================================================

class Simulation
{
public:

    ~Simulation();
    Simulation(const SimulationConfig& cfg);

    void assemble();
    void solve();

    // -------------------------
    // external accessors
    // -------------------------
    const MeshBase& mesh() const                                        { return *mesh_; }
    const LinearEquationSystem& system() const                                  { return sys_; }
    const BoundaryPatchSystem& boundary() const                         { return boundary_; }
    const ScalarTransportModel& model() const noexcept                          { return *physics_; }

    VerificationCase* verificationCase()                                { return verificationCase_.get(); }
    const VerificationCase* verificationCase() const                    { return verificationCase_.get(); }
    
    void printMesh() const
    {
        for (std::size_t f = 0; f < mesh_->nfaces(); ++f) {
            std::cout << "Mesh face: " << f
                    << " owner=" << mesh_->face(f).owner
                    << " neighbor=" << mesh_->face(f).neighbor << "\n";
        }
    }

    void setVerificationCase(std::unique_ptr<VerificationCase> verificationCase);    
    FieldRegistry& fields()                                             { return fields_; }
    const FieldRegistry& fields() const                                 { return fields_; }
    const ScalarField& solution() const;
    
private:
    // -------------------------
    // internal wiring
    // -------------------------
    void bindBoundaryConditions(const SimulationConfig& cfg);
    void initializeFields();

    std::unique_ptr<MeshBase> mesh_;

    BoundaryPatchSystem boundary_;
    std::unique_ptr<ScalarTransportModel> physics_;

    LinearSystem sys_;
    std::unique_ptr<FluxAccumulator> flux_;

    StandardDiffusionScheme diffusionScheme_;

    std::unique_ptr<GradientScheme> gradientScheme_;
    std::unique_ptr<ReconstructionScheme> reconstructionScheme_;

    std::unique_ptr<VectorField> gradient_;

    ConvectionOperator convection_;
    DiffusionOperator diffusion_;
    FiniteVolumeAssembler fvOperator_;
    
    SimulationConfig cfg_;
    bool assembled_;
    
    std::unique_ptr<VerificationCase> verificationCase_;
    FieldRegistry fields_;
};
