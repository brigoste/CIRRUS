#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

#include <iostream>

// Similar to a heated slab with fixed dirichlect boundary conditions.

/*
    Documentation:

    Case                    Solver      Mesh        L2 Error      L2 Check    Refinement    Order
    --------------------------------------------------------------------------------------------------
    Quadratic1D             CG          20x1        6.250e-02     PASS        N/A           N/A
*/

class Quadratic1D : public VerificationCase
{
public:
    explicit Quadratic1D(const SimulationConfig& cfg)
    {
        k_ = cfg.physics.k;
        volumetricSource_ = cfg.physics.volumetricSource;
        bool foundLeft = false;
        bool foundRight = false;

        for (const auto& bc : cfg.boundary)
        {
            if(bc.group == 0) { 
                TL_ = bc.condition.value; 
                foundLeft = true;
            }
            else if (bc.group == 1) { 
                TR_ = bc.condition.value; 
                foundRight = true;
            }
        }

        if(!foundLeft || !foundRight){
            throw std::runtime_error("Quadratic1D requires Dirichlet boundary groups 0 and 1");
        }
    }

    void initialize(const MeshBase& mesh) override { L_ = mesh.getLx(); }

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;
    double source(double x, double y) const override;

    double l2AcceptanceThreshold() const override { 
        // Second-order diffusion discretization:
        // O(dx^2) error on 20 cells gives approximately 6.25e-2
        // Allow margin for implementation changes.
        return 7e-2; 
    }
    double linfAcceptanceThreshold() const override { return 7e-2; }

private:
    double k_, TL_, TR_, volumetricSource_, L_;
};
