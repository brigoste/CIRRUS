#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

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
    explicit Quadratic1D(const nlohmann::json& params) {
        if (!params.contains("k")) { throw std::runtime_error("Quadratic1D missing required parameter k"); }
            k_ = params.value("k",100);

        if (!params.contains("TL")) { throw std::runtime_error("Quadratic1D missing reuqired parameter TL"); }
            TL_ = params.value("TL",300);

        if (!params.contains("TR")) { throw std::runtime_error("Quadratic1D missing reuqired parameter TR"); }
            TR_ = params.value("TR",400);

        if (!params.contains("qdot")) { throw std::runtime_error("Quadratic1D missing reuqired parameter qdot"); }
            qdot_ = params.value("qdot", 40000.0);
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
    double k_, TL_, TR_, qdot_, L_;
};
