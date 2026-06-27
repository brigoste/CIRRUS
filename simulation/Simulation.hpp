#pragma once

#include <memory>
#include <string>
#include <iostream>

#include "config/SimulationConfig.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "physics/PhysicsModel.hpp"
#include "physics/PhysicsFactory.hpp"
#include "physics/HeatPhysicsModel.hpp"
#include "linear_system/LinearSystem.hpp"
#include "bc/BoundaryFace.hpp"
#include "discretization/FluxBuilder.hpp"
#include "Solver/CG.hpp"
#include "Solver/SOR.hpp"
#include "Solver/TDMA.hpp"
#include "Solver/GaussSeidel.hpp"
#include "Solver/BiCGSTAB.hpp"
#include "Solver/SolverMethod.hpp"
// #include "tests\verification\VerificationRegistry.hpp"

// ============================================================
// Simulation = composition root
// ============================================================

class Simulation
{
public:
    Simulation(const SimulationConfig& cfg);

    void assemble();
    std::vector<double> solve();

    // -------------------------
    // external accessors
    // -------------------------
    const MeshBase& mesh() const { return *mesh_; }
    const LinearSystem& system() const { return sys_; }
    const BoundaryPatchSystem& boundary() const { return boundary_; }
    const PhysicsModel& model() const noexcept { return *model_; }
    const VerificationCase* verificationCase() const { return verificationCase_.get(); }
    
    void printMesh() const
    {
        for (std::size_t f = 0; f < mesh_->nfaces(); ++f) {
            std::cout << "Mesh face: " << f
                    << " owner=" << mesh_->face(f).owner
                    << " neighbor=" << mesh_->face(f).neighbor << "\n";
        }
    }

    void setVerificationCase(std::unique_ptr<VerificationCase> verificationCase);

    

private:
    // -------------------------
    // internal wiring
    // -------------------------
    void bindBoundaryConditions(const SimulationConfig& cfg);
    std::size_t resolveBoundaryFace(const std::string& loc);

    std::unique_ptr<MeshBase> mesh_;

    BoundaryPatchSystem boundary_;
    std::unique_ptr<PhysicsModel> model_;
    LinearSystem sys_;
    std::unique_ptr<FluxAccumulator> flux_;
    SimulationConfig cfg_;
    bool assembled_;
    // VerificationRegistry verificationRegistry_;
    std::unique_ptr<VerificationCase> verificationCase_;
};
