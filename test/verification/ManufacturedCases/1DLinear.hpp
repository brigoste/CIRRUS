#pragma once

#include "tests/verification/VerificationCase.hpp"

class OneDLinear : public VerificationCase
{
public:
    explicit OneDLinear(const nlohmann::json& params)
    {
        (void) params;
    }
    void initialize(const MeshBase&) override {}  // Default: nothing needed 
    

    double exact( double x, double y) const override;

    double laplacian( double x, double y) const override;

    double source(double , double ) const override { return 0.0; }

    double l2AcceptanceThreshold() const override { return 1e-10; }
    double linfAcceptanceThreshold() const override { return 1e-10; }
};
