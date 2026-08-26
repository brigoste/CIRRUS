#pragma once

#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/MeshBase.hpp"
#include "physics/PhysicsModel.hpp"
#include "discretization/FluxAccumulator.hpp"

class ConvectionFluxBuilder
{
public:
    void apply(
        const MeshBase& mesh,
        const PhysicsModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux
    ) const;
};
