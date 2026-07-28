#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"
#include "utils/MathConstants.hpp"

/*
    Documentation:

    Case                    Solver      Mesh        L2 Error      L2 Check    Refinement    Order
    --------------------------------------------------------------------------------------------------
    AdvectionDiffusion2D    BiCGSTAB    40x40       4.124e-03     PASS        PASS          1.0
*/

class AdvectionDiffusion2D : public VerificationCase
{
public:
    explicit AdvectionDiffusion2D(const SimulationConfig& cfg)
    {
        gamma_ = cfg.physics.gamma;
        rho_   = cfg.physics.rho;
        ux_    = cfg.physics.ux;
        uy_    = cfg.physics.uy;
    }
    void initialize(const MeshBase&) override {}// Default: nothing needed     

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;
    double source(double x, double y) const override;

    double l2AcceptanceThreshold() const override { return 1e-2; }
    double linfAcceptanceThreshold() const override { return 2e-2; }

private:
    double rho_, ux_, uy_, gamma_;
};
