#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

/*
    Documentation:

    Case                    Solver      Mesh          L2 Error      L2 Check    Refinement    Order
    ----------------------------------------------------------------------------------------------------
    Neumann2D               CG          200x200       3.125e-04     PASS        PASS          2.000e+00

    Manufactured solution:
        Quadratic temperature field with:
            - One Dirichlet boundary
            - Three spatially varying Neumann flux boundaries
            - Constant volumetric source term

    Validation:
        L2 acceptance threshold      : 5e-2
        Linf acceptance threshold    : 5e-2
        Expected convergence order  : 2.0

        Refinement sequence:
            N = {25, 50, 100, 200}

        Observed convergence:
            L2 order   = 2.0
            Linf order = 2.0
*/

class Neumann2D : public VerificationCase
{
public:

    explicit Neumann2D(const SimulationConfig& cfg)
    {
        k_ = cfg.physics.k;

        T0_ = 100.0;
        a_ = 10.0;
        b_ = -5.0;
        c_ = 50.0;
        d_ = -25.0;

        bool hasDirichlet = false;

        for (const auto& bc : cfg.boundary)
        {
            if (bc.group == 0) { hasDirichlet = true; }
        }

        if (!hasDirichlet)
        {
            throw std::runtime_error( "Neumann2D requires at least one Dirichlet boundary." );
        }
    };

    void initialize(const MeshBase&) override {}

    double exact(double x, double y = 0.0) const override;

    double source(double x, double y = 0.0) const override;

    double laplacian(double x, double y = 0.0) const override;

    double boundaryFlux(const Face& face) const override;
    
    double l2AcceptanceThreshold() const override
    {
        return 5e-2;
    }

    double linfAcceptanceThreshold() const override
    {
        return 5e-2;
    }

private:

    double k_, volumetricSource_, Lx_, Ly_, T0_, a_, b_, c_, d_;
};
