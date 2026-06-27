#pragma once
#include "Physics/HeatPhysicsModel.hpp"
#include "config/SimulationConfig.hpp"
#include "nlohmann/json.hpp"

// Abstraction for all tests we use to verify.

class VerificationCase
{
public:
    virtual ~VerificationCase() = default;

    virtual double exact(double x, double y) const = 0;

    virtual double laplacian(double x, double y) const = 0;

    virtual double source(double x, double y) const = 0;
};
