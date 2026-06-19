#pragma once
#include "Physics/HeatPhysicsModel.hpp"
#include "config/SimulationConfig.hpp"

// Abstraction for all tests we use to verify.

#pragma once

class VerificationCase
{
public:
    virtual ~VerificationCase() = default;

    virtual SimulationConfig config() const = 0;

    // Manufactured solution
    virtual double exact(double x, double y) const = 0;

    // RHS of PDE: -∇·(k∇φ) = S (WITHOUT k if your physics applies it)
    virtual double laplacian(double x, double y) const = 0;
};
