#pragma once

#include "system/LinearSystem.hpp"
#include <vector>

std::vector<double> SOR(
    LinearSystem& sys,
    int iter = 1000,
    double tol = 1e-5,
    double omega = 1.2,
    bool output = false
);
