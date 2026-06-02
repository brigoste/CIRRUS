#pragma once
#include "linear_system/LinearSystem.hpp"
#include <vector>

std::vector<double> GaussSeidel(
    const LinearSystem sys,
    int max_iter,
    double tol);
