#pragma once

#include "test/verification/VerificationCase.hpp"

/*
    Documentation:

    Case                    Solver      Mesh        L2 Error      Accuracy    Convergence   Order
    --------------------------------------------------------------------------------------------------
    Robin1D            CG          200x1       6.250×10⁻⁶    PASS        PASS          2.000e+00
*/

class Robin1D : public VerificationCase
{
public:
    explicit Robin1D(const SimulationConfig& cfg)
        : k_(cfg.physics.transferCoefficient)
    {}

    void initialize(const MeshBase&) override {}

    double exact(Point p) const override;

    double laplacian(Point p) const override;

    double source(Point p) const override;

    RobinData manufacturedRobinBoundary(const Face& face) const override;

    double l2AcceptanceThreshold() const override                       { return 5e-2; }

    double linfAcceptanceThreshold() const override                     { return 5e-2; }

private:
    double k_;
};
