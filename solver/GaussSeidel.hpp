#pragma once

#include <vector>

class LinearEquationSystem;

std::vector<double> GaussSeidel(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol);