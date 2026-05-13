#pragma once

#include "linear_system/LinearSystem.hpp"
#include "mesh/MeshBase.hpp"
#include <vector>

std::vector<double> GaussSeidel(
    LinearSystem& sys,
    [[maybe_unused]]const MeshBase& mesh,
    int iter = 1000,
    double tol = 1e-8,
    bool output = false
);
