#pragma once

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "physics/HeatEquationModel.hpp"

class FiniteVolumeOperator
{
public:
    static void assemble(
        const MeshBase& mesh,
        const HeatEquationModel& model,
        LinearSystem& sys);
};
