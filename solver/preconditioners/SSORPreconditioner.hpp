#pragma once

#include "solver/preconditioners/Preconditioner.hpp"
#include "equation_systems/LinearEquationSystem.hpp"

#include <vector>

class SSORPreconditioner : public Preconditioner
{
public:
    explicit SSORPreconditioner(double omega = 1.0);

    void setup(const LinearEquationSystem& sys) override;

    void apply(const std::vector<double>& r,
               std::vector<double>& z) const override;

    std::string name() const override { return "SSOR"; }

private:
    double omega_;
    const LinearEquationSystem* system_ = nullptr;
    std::vector<double> diagonal_;
};
