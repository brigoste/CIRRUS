#pragma once

#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/MeshBase.hpp"
#include "physics/ScalarTransport/ScalarTransportModel.hpp"
#include "discretization/FluxAccumulator.hpp"

class ConvectionFluxBuilder
{
public:
    void apply(
        const MeshBase& mesh,
        const ScalarTransportModel& model,
        const BoundaryPatchSystem& boundary,
        FluxAccumulator& flux
    ) const;
};
