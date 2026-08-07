#pragma once

#include "mesh/MeshBase.hpp"
#include "physics/PhysicsModel.hpp"
#include "discretization/FluxAccumulator.hpp"

class DiffusionOperator
{
public:

    void apply(
        const MeshBase& mesh,
        const PhysicsModel& model,
        FluxAccumulator& flux
    ) const;
};