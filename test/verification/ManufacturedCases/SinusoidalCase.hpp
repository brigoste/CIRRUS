#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

class Sinusoidal2D : public VerificationCase
{
public:
    SimulationConfig config() const override;

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;
};
