#pragma once

#include "system/LinearSystem.hpp"
#include <vector>

std::vector<double> GaussSeidel(
    LinearSystem& sys,
    int iter = 1000,
    double tol = 1e-5,
    bool output = false
);
