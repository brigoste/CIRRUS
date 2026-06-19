#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

class Quadratic1D : public VerificationCase
{
public:
    SimulationConfig config() const override;

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;
};
