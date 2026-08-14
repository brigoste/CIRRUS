#pragma once

#include "test/verification/VerificationCase.hpp"

/*
    Documentation:

    Case              Solver      Mesh        L2 Error      L2 Check    Refinement    Order
    --------------------------------------------------------------------------------------------------
    Convective2D      CG          256x256     6.409e-05     PASS        PASS          2.001e+00 
*/

class Convective2D : public VerificationCase
{
public:

    explicit Convective2D(const SimulationConfig& cfg)
        :
        k_(cfg.physics.k)
    {
        convectiveBC_.transferCoefficient = 10.0;
        convectiveBC_.referenceValue = 93.0;
    }


    void initialize(const MeshBase&) override {}

    double exact(double x, double y) const override;

    double laplacian(double x, double y) const override;

    double source(double x, double y) const override;

    double manufacturedBoundaryFlux(const Face& face) const override;

    ConvectiveData manufacturedRobinBoundary(const Face& face) const override;

    double l2AcceptanceThreshold() const override                       { return 1e-3; }
    double linfAcceptanceThreshold() const override                     { return 2e-3; }

private:

    double k_;

    ConvectiveData convectiveBC_;

    // Manufactured solution:
    //
    // T(x,y) = T0 + a*x + c*x^2 + d*cos(pi*y/Ly)

    const double T0_ = 100.0;
    const double a_  = 5.0;
    const double c_  = -2.0;
    const double d_  = 10.0;
    const double Ly_ = 1.0;
};
