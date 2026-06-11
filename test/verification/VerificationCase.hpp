#pragma once
#include "Physics/HeatPhysicsModel.hpp"

// Abstraction for all tests we use to verify.

class VerificationCase
{
public:
    virtual ~VerificationCase() = default;

    // Exact solution φ(x,y)
    virtual double exact(
        double x,
        double y) const = 0;

    // Source term S(x,y)
    virtual double source(
        double x, 
        double y) const = 0;
};
