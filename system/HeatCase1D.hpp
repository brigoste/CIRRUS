#pragma once

#include <vector>
#include <functional>

#include "Solver/SolverMethod.hpp"
#include "physics/HeatEquationModel.hpp"
#include "config/BoundaryConfig.hpp"

struct HeatCase1D
{
    std::size_t n;
    double L;
    double A;
    double k;

    std::function<double(const Point&)> Su = nullptr;
    std::function<double(const Point&)> Sp = nullptr;

    // ✔ PURE INPUT LAYER (no mesh dependency)
    std::vector<BoundaryConfig> bcs;
};
