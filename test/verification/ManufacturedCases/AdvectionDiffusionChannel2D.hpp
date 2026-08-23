#pragma once

#include "test/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"
#include "utils/MathConstants.hpp"

/*
    Documentation:

    Case                    Solver      Mesh        L2 Error      L2 Check    Refinement    Order
    --------------------------------------------------------------------------------------------------
    AdvectionDiffusionChannel2D    BiCGSTAB    320x320     5.731e-04     PASS        PASS          9.491e-01
*/

class AdvectionDiffusionChannel2D : public VerificationCase
{
public:
    explicit AdvectionDiffusionChannel2D(const SimulationConfig& cfg)
    {
        gamma_ = cfg.physics.gamma;
        rho_   = cfg.physics.rho;
        ux_    = cfg.physics.ux;
        uy_    = cfg.physics.uy;
    }
    void initialize(const MeshBase&) override {}// Default: nothing needed     

    double exact(Point p) const override;
    double laplacian(Point p) const override;
    double source(Point p) const override;

    double l2AcceptanceThreshold() const override { return 1e-2; }
    double linfAcceptanceThreshold() const override { return 2e-2; }
    double manufacturedBoundaryFlux( const Face& face ) const override;

private:
    double rho_, ux_, uy_, gamma_;
};
