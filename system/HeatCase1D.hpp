#pragma once

#include <vector>
#include <functional>

#include "Solver/SolverMethod.hpp"
#include "bc/BoundaryConditionDescriptor.hpp"
#include "physics/HeatEquationModel.hpp"

struct HeatCase1D
{
    int n;
    double L;
    double A;
    double k;

    double Su;
    double Sp;

    std::vector<BoundaryConditionDescriptor> bcs;
};
