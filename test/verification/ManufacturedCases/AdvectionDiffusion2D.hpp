#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"
#include "utils/MathConstants.hpp"

class AdvectionDiffusion2D : public VerificationCase
{
public:
    explicit AdvectionDiffusion2D(const nlohmann::json& params)
    {
        if (!params.contains("gamma"))
            throw std::runtime_error("AdvectionDiffusion2D missing required parameter gamma");
        if (!params.contains("rho"))
            throw std::runtime_error("AdvectionDiffusion2D missing required parameter rho");
        if (!params.contains("ux"))
            throw std::runtime_error("AdvectionDiffusion2D missing required parameter ux");
        if (!params.contains("uy"))
            throw std::runtime_error("AdvectionDiffusion2D missing required parameter uy");

        gamma_ = params.value("gamma", 1.0);
        rho_ = params.value("rho", 1.0);
        ux_ = params.value("ux", 1.0);
        uy_ = params.value("uy", 0.0);
    }

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;
    double source(double x, double y) const override;

private:
    double rho_, ux_, uy_, gamma_;
};
