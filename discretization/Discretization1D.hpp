#pragma once
#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"

void discretize_1D(
    const Mesh1D& mesh,
    double k,
    double A,
    Coefficients1D& c
);
