#pragma once

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "physics/PhysicsModel.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "discretization/CellFlux.hpp"
#include "discretization/CellData.hpp"
#include "bc/BCType.hpp"

// #pragma message("USING FV HEADER: " __FILE__)

class FiniteVolumeOperator
{
public:
    static void assemble( const FluxAccumulator& flux, LinearSystem& sys);
};
