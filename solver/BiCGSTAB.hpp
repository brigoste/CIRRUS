#pragma once
#include "linear_system/LinearSystem.hpp"
#include "solver/preconditioners/Preconditioner.hpp"
#include <vector>

std::vector<double> BiCGSTAB(
    const LinearSystem& sys,
    int max_iter,
    double tol,
    const Preconditioner& M);
