#pragma once

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "physics/PhysicsModel.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "discretization/CellFlux.hpp"
#include "discretization/CellData.hpp"
#include "discretization/convection/ConvectionScheme.hpp"
#include "bc/BCType.hpp"

class FiniteVolumeOperator
{
public:

    FiniteVolumeOperator(
        const ConvectionScheme& convectionScheme
    )
    :
        convectionScheme_(convectionScheme)
    {}

    void assemble(
        const FluxAccumulator& flux,
        LinearSystem& sys
    ) const;

private:

    const ConvectionScheme& convectionScheme_;
};
