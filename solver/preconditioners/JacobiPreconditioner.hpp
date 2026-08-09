#pragma once

#include "solver/preconditioners/Preconditioner.hpp"
#include "equation_systems/LinearEquationSystem.hpp"

#include <vector>

class JacobiPreconditioner : public Preconditioner
{
public:

    void setup(const LinearEquationSystem& sys) override;

    void apply(
        const std::vector<double>& r,
        std::vector<double>& z) const override;

        std::string name() const override { return "Jacobi"; }

private:

    std::vector<double> diagInv_;
};