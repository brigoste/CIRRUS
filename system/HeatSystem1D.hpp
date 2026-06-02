#pragma once

#include <vector>

// Core mesh
#include "mesh/MeshBase.hpp"
#include "mesh/Face.hpp"

// Physics + numerics
#include "physics/HeatEquationModel.hpp"
#include "linear_system/LinearSystem.hpp"

// BCs (still needed for flux construction stage)
#include "mesh/BoundaryPatchSystem.hpp"

// Flux layer
#include "discretization/FluxAccumulator.hpp"

// Interface
#include "system/Interfaces/HeatSystemBase.hpp"

class HeatSystem1D : public HeatSystemBase
{
public:
    HeatSystem1D(const MeshBase& mesh,
                 const BoundaryPatchSystem& boundary);

    // -------------------------------------------------
    // assembly / solve lifecycle
    // -------------------------------------------------
    void assemble(const HeatEquationModel& model) override;

    std::vector<double> solve(
        solver::Method method,
        int max_iter,
        double tol,
        double omega = 1.0
    ) override;

    // -------------------------------------------------
    // interface compliance
    // -------------------------------------------------
    const MeshBase& mesh() const override;
    const LinearSystem& system() const override;

    int size() const override;

private:
    const MeshBase& mesh_;
    LinearSystem sys_;
    const BoundaryPatchSystem& boundary_;
    FluxAccumulator flux_;
};
