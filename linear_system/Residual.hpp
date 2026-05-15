#pragma once

#include <vector>

class LinearSystem;

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const std::vector<double>& x);
