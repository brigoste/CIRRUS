#pragma once

#include "test/verification/VerificationCase.hpp"

/*
    Documentation:

    Case                    Solver      Mesh        L2 Error      L2 Check    Refinement    Order
    --------------------------------------------------------------------------------------------------
    Linear1D                TDMA        20x1        2.049e-13     PASS        N/A           N/A
*/

class OneDLinear : public VerificationCase
{
public:
    OneDLinear() = default;
    
    void initialize(const MeshBase&) override {}  // Default: nothing needed 
    
    double exact( double x, double y) const override;
    double laplacian( double x, double y) const override;
    double source(double , double ) const override                  { return 0.0; }

    double l2AcceptanceThreshold() const override                   { return 1e-10; }
    double linfAcceptanceThreshold() const override                 { return 1e-10; }

private:
    double k_;
};
