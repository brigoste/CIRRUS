#pragma once
#include "linear_system/LinearSystem.hpp"
#include <vector>

std::vector<double> CG( const LinearSystem& sys, int max_iter, double tol);
