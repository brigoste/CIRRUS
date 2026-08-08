#pragma once
#include "equation_systems/LinearEquationSystem.hpp"
#include "solver/preconditioners/Preconditioner.hpp"
#include <vector>

std::vector<double> BiCGSTAB(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    const Preconditioner& M);
