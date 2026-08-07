#pragma once
#include "physics/HeatPhysicsModel.hpp"
#include "config/SimulationConfig.hpp"
#include "nlohmann/json.hpp"
#include "mesh/MeshBase.hpp"

#include <stdexcept>

// Abstraction for all tests we use to verify.
struct ConvectiveData
{
    double h;
    double T_inf;
};

struct RadiativeData
{
    double emissivity;
    double sigma;
    double T_inf;
};

class VerificationCase
{
public:
    virtual ~VerificationCase() = default;

    virtual void initialize(const MeshBase& mesh) = 0;

    virtual double exact(double x, double y) const = 0;
    virtual double laplacian(double x, double y) const = 0;
    virtual double source(double x, double y) const = 0;

    virtual double boundaryFlux(const Face&) const
    {
        throw std::runtime_error(
            "Neumann boundary flux not implemented for this verification case."
        );
    }
    virtual ConvectiveData boundaryConvective(const Face&) const
    {
        throw std::runtime_error(
            "Convective boundary data not implemented for this verification case."
        );
    }
    virtual RadiativeData boundaryRadiative(const Face&) const
    {
        throw std::runtime_error(
            "Radiative boundary data not implemented for this verification case."
        );
    }

    virtual double l2AcceptanceThreshold() const = 0;
    virtual double linfAcceptanceThreshold() const = 0;
};
