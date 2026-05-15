#pragma once
#include "bc/BCType.hpp"
#include "mesh/Point.hpp"

struct Face
{
    std::size_t owner;
    std::size_t neighbor = INVALID;

    Point center;
    Point normal;
    double area = 0.0;
    double centroidDistance = 0.0;

    bool isBoundary = false;

    // 🔥 SINGLE SOURCE OF TRUTH
    BCType bcType = BCType::Dirichlet;

    double value = 0.0;  // Dirichlet
    double flux  = 0.0;  // Neumann
    double h     = 0.0;  // Convective
    double Tinf  = 0.0;
};
