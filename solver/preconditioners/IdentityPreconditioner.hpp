#pragma once

#include "solver/preconditioners/Preconditioner.hpp"

class IdentityPreconditioner : public Preconditioner
{
public:

    void setup(const LinearEquationSystem& sys) override;
    void apply( const std::vector<double>& r, std::vector<double>& z) const override;
    std::string name() const override { return "None"; }
};
