#pragma once

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "physics/HeatEquationModel.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "discretization/CellFlux.hpp"
#include "discretization/CellData.hpp"
#include "bc/BCType.hpp"

class FiniteVolumeOperator
{
public:
    static void assemble(
        const MeshBase& mesh,
        const HeatEquationModel& model,
        LinearSystem& sys);
};
