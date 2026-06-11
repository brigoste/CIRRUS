#pragma once

#include "tests/verification/VerificationCase.hpp"

class OneDLinear : public VerificationCase
{
public:

    double exact(
        double x,
        double y = 0.0) const override;

    double source(
        double x,
        double y = 0.0) const override;
};
