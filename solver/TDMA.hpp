#pragma once

#include "equation_systems/LinearSystem.hpp"
#include <vector>

// 1D-only solver (tridiagonal system)
std::vector<double> TDMA( const LinearSystem& sys );
