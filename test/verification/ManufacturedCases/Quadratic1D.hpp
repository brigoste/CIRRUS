#pragma once

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

class Quadratic1D : public VerificationCase
{
public:
    explicit Quadratic1D(const nlohmann::json& params) {
        if (!params.contains("k")) { throw std::runtime_error("Quadratic1D missing required parameter k"); }
        k_ = params.value("k",100);
    }

    double exact(double x, double y) const override;
    double laplacian(double x, double y) const override;
    double source(double x, double y) const override;

private:
    double k_;
};
