#pragma once
#include "linear_system/LinearSystem.hpp"
#include <vector>

std::vector<double> BiCGSTAB(
    const LinearSystem& sys,
    int max_iter,
    double tol);
