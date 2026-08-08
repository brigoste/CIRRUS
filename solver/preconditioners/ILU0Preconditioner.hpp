#pragma once

#include "solver/preconditioners/Preconditioner.hpp"

#include <vector>
#include <unordered_map>

class ILU0Preconditioner : public Preconditioner
{
public:

    void copyMatrix(const LinearEquationSystem& sys);
    void setup(const LinearEquationSystem& sys);
    void factorize();

    void apply( const std::vector<double>& r, std::vector<double>& z) const override;

    std::string name() const override { return "ILU0"; }

private:

    // L and U storage
    std::vector<std::unordered_map<std::size_t,double>> LU_;

    std::size_t N_ = 0;
};
