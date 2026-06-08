#pragma once

#include "tests/verification/VerificationCase.hpp"

class SinusoidalCase : public VerificationCase
{
public:

    double exact(
        double x,
        double y) const override;

    double source(
        double x,
        double y) const override;
};
