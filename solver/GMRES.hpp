#pragma once

#include "equation_systems/LinearEquationSystem.hpp"
#include "solver/preconditioners/Preconditioner.hpp"

#include <vector>

std::vector<double> GMRES(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    int restart,
    const Preconditioner& M);