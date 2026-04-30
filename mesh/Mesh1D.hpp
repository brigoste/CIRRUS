#pragma once
#include "bc/BoundaryFace.hpp"

#include <vector>

struct Mesh1D {
    // --- grid definition ---
    int n;          // number of nodes
    double L;       // domain length
    double dx;      // uniform spacing

    // --- material / physics properties ---
    double A;       // cross-sectional area
    double k;       // thermal conductivity

    // --- geometry ---
    std::vector<double> x;

    // --- constructor ---
    Mesh1D(int n_, double L_, double A_, double k_);

    BoundaryFace faceType(int i) const;
};
