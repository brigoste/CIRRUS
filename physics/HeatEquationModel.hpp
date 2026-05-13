#pragma once

#include <functional>
#include "mesh/MeshBase.hpp"

struct HeatEquationModel
{
    double k = 1.0;
    double Su;
    double Sp;

    // std::function<double(const MeshPoint&)> Su = nullptr;
    // std::function<double(const MeshPoint&)> Sp = nullptr;
};
