#pragma once

#include "equation_systems/LinearEquationSystem.hpp"

#include <vector>

// 1D-only solver (tridiagonal system)
std::vector<double> TDMA(const LinearEquationSystem& sys);