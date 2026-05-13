#pragma once

#include "linear_system/LinearSystem.hpp"
#include <vector>

std::vector<double> CG(
    LinearSystem& sys,
    int iter,
    double tol
);
