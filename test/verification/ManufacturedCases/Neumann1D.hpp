#pragma once

#include "test/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

/*
    Documentation:

    Case          Solver   Mesh       L2 Error      L2 Check   Refinement   Order
    -------------------------------------------------------------------------------
    Neumann1D     TDMA     200x1      3.125e-4     PASS       PASS         2.000e+00
*/
class Neumann1D : public VerificationCase
{
public:
    explicit Neumann1D(const SimulationConfig& cfg)
    {
        k_ = cfg.physics.transferCoefficient;
        volumetricSource_ = cfg.physics.volumetricSource;
        bool foundLeft = false;
        bool foundRight = false;
        L_ = cfg.mesh.lx;
        nx_ = cfg.mesh.nx;

         for (const auto& bc : cfg.boundary)
        {
            if(bc.group == 0) { 
                TL_ = bc.condition.value; 
                foundLeft = true;
            }
            else if (bc.group == 1)
            {
                qR_ = bc.condition.flux;
                foundRight = true;
            }
        }

        if(!foundLeft || !foundRight)
        {
            throw std::runtime_error("Neumann1D requires Dirichlet group 0 and Neumann group 1.");
        }
    }

    void initialize(const MeshBase&) override {}

    double exact(double x, double y = 0.0) const override;

    double source(double x, double y = 0.0) const override;

    double laplacian(double x, double y = 0.0) const override;

    double l2AcceptanceThreshold() const override       { return 1e-3; }
    double linfAcceptanceThreshold() const override     { return 1e-3; }
    double manufacturedBoundaryFlux(const Face&) const override     { return qR_; }

private:

    double k_, volumetricSource_, TL_, qR_, L_;
    int nx_;
};

/*
    Manufactured solution:

        -k d2T/dx2 = q'''

    Boundary conditions:

        T(0) = TL

        k dT/dx(L) = qR

    Note:
        CIRRUS Neumann BC convention uses flux sign such that
        the boundary contribution is added as:

            Su = -flux * area

        Therefore the manufactured solution uses:

            C1 = (-qR + q'''L)/k

    Expected behavior:

        Second-order spatial convergence.
*/
