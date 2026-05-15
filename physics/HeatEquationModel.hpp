#pragma once

#include <functional>
#include "mesh/Point.hpp"

struct HeatEquationModel
{
    double k = 1.0;
    bool hasConvection = false;

    // volumetric source decomposition:
    std::function<double(const Point&)> Su = nullptr;
    std::function<double(const Point&)> Sp = nullptr;

};
