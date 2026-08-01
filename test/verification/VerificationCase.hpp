#pragma once
#include "Physics/HeatPhysicsModel.hpp"
#include "config/SimulationConfig.hpp"
#include "nlohmann/json.hpp"

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
        throw std::runtime_error( "Neumann2D requires at least one Dirichlet boundary group." );
    }
    virtual ConvectiveData boundaryConvective(const Face&) const
    {
        throw std::runtime_error("Robin BC not implemented.");
    }

    virtual double l2AcceptanceThreshold() const = 0;
    virtual double linfAcceptanceThreshold() const = 0;
};
