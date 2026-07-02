#pragma once

#include "tests/verification/VerificationCase.hpp"

class OneDLinear : public VerificationCase
{
public:
    explicit OneDLinear(const nlohmann::json& params)
    {
        (void) params;
    }

    double exact( double x, double y) const override;

    double laplacian( double x, double y) const override;

    double source(double , double ) const override { return 0.0; }
};
