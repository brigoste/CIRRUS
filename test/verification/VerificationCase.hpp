#pragma once
#include "config/SimulationConfig.hpp"
#include "mesh/MeshBase.hpp"

#include <stdexcept>

// Abstraction for all tests we use to verify.
struct RobinData
{
    double transferCoefficient;
    double referenceValue;
};

class VerificationCase
{
public:
    virtual ~VerificationCase() = default;

    virtual void initialize(const MeshBase& mesh) = 0;

    virtual double exact(double x, double y) const = 0;
    virtual double laplacian(double x, double y) const = 0;
    virtual double source(double x, double y) const = 0;

    virtual double manufacturedBoundaryFlux(const Face&) const
    {
        throw std::runtime_error(
            "Neumann boundary flux not implemented for this verification case."
        );
    }
    virtual RobinData manufacturedRobinBoundary(const Face&) const
    {
        throw std::runtime_error(
            "Robin boundary data not implemented for this verification case."
        );
    }

    virtual double l2AcceptanceThreshold() const = 0;
    virtual double linfAcceptanceThreshold() const = 0;
};
