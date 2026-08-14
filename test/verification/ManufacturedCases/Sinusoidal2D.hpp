#pragma once

#include "test/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

/*
    Documentation:
    
    Case                    Solver      Mesh        L2 Error      L2 Check    Refinement    Order
    --------------------------------------------------------------------------------------------------
    Sinusoidal2D            CG          50x50       6.585e-04     PASS        PASS          2.00e+00

*/

class Sinusoidal2D : public VerificationCase
{
public:
    explicit Sinusoidal2D(const SimulationConfig& cfg)
        : k_(cfg.physics.transferCoefficient)
    {}

    void initialize(const MeshBase&) override {}// Default: nothing needed 

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;

    double source(double x, double y) const override;

    double l2AcceptanceThreshold() const override       { return 5e-3; }
    double linfAcceptanceThreshold() const override     { return 5e-3; }
private:
    double k_;
};
