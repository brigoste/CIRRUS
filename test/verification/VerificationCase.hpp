#pragma once
#include "physics/HeatPhysicsModel.hpp"
#include "config/SimulationConfig.hpp"
#include "nlohmann/json.hpp"

// Abstraction for all tests we use to verify.

class VerificationCase
{
public:
    virtual ~VerificationCase() = default;

    virtual void initialize(const MeshBase& mesh) = 0;

    virtual double exact(double x, double y) const = 0;
    virtual double laplacian(double x, double y) const = 0;
    virtual double source(double x, double y) const = 0;

    virtual double l2AcceptanceThreshold() const = 0;
    virtual double linfAcceptanceThreshold() const = 0;
};
