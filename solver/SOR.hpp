#pragma once

#include <vector>

class LinearEquationSystem;

std::vector<double> SOR(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    double omega);