#pragma once

#include "test/verification/VerificationCase.hpp"

/*
    Documentation:

    Case                    Solver      Mesh        L2 Error      Accuracy    Convergence   Order
    --------------------------------------------------------------------------------------------------
    Convective1D            CG          200x1       6.250×10⁻⁶    PASS        PASS          2.000e+00
*/

class Convective1D : public VerificationCase
{
public:
    explicit Convective1D(const SimulationConfig& cfg)
        : k_(cfg.physics.k)
    {}

    void initialize(const MeshBase&) override {}

    double exact(double x, double y) const override;

    double laplacian(double x, double y) const override;

    double source(double x, double y) const override;

    ConvectiveData boundaryConvective(const Face& face) const override;

    double l2AcceptanceThreshold() const override                       { return 5e-2; }

    double linfAcceptanceThreshold() const override                     { return 5e-2; }

private:
    double k_;
};
