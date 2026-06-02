#pragma once
#include "discretization/FluxAccumulator.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "physics/HeatEquationModel.hpp"

class FluxBuilder
{
public:
    static void buildFlux(
        const MeshBase& mesh,
        const HeatEquationModel& model,
        const BoundaryPatchSystem& bc,
        FluxAccumulator& flux);
};
