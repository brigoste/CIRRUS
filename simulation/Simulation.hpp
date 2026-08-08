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
#include "physics/HeatPhysicsModel.hpp"
#include "linear_system/LinearSystem.hpp"
#include "bc/BoundaryFace.hpp"

#include "discretization/convection/UpwindScheme.hpp"
#include "discretization/convection/CentralDifferenceScheme.hpp"
#include "discretization/diffusion/DiffusionOperator.hpp"
#include "discretization/operators/GradientOperator.hpp"
#include "discretization/operators/ConvectionOperator.hpp"
#include "discretization/DiffusionFluxBuilder.hpp"
#include "discretization/FiniteVolumeOperator.hpp"
#include "discretization/FluxBuilder.hpp"

#include "solver/CG.hpp"
#include "solver/SOR.hpp"
#include "solver/TDMA.hpp"
#include "solver/GaussSeidel.hpp"
#include "solver/BiCGSTAB.hpp"
#include "solver/SolverMethod.hpp"

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
    const LinearSystem& system() const                                  { return sys_; }
    const BoundaryPatchSystem& boundary() const                         { return boundary_; }
    const PhysicsModel& model() const noexcept                          { return *physics_; }

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
    
private:
    // -------------------------
    // internal wiring
    // -------------------------
    void bindBoundaryConditions(const SimulationConfig& cfg);
    void initializeFields();
    // std::size_t resolveBoundaryFace(const std::string& loc);

    std::unique_ptr<MeshBase> mesh_;

    BoundaryPatchSystem boundary_;
    std::unique_ptr<PhysicsModel> physics_;

    LinearSystem sys_;
    std::unique_ptr<FluxAccumulator> flux_;

    CentralDifferenceScheme convectionScheme_;

    ConvectionOperator convection_;
    DiffusionOperator diffusion_;
    FiniteVolumeOperator fvOperator_;
    
    std::unique_ptr<GradientOperator> gradient_;
    
    
    SimulationConfig cfg_;
    bool assembled_;
    // VerificationRegistry verificationRegistry_;
    
    std::unique_ptr<VerificationCase> verificationCase_;
    FieldRegistry fields_;
};
