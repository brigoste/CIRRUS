#include "equation_systems/Residual.hpp"

#include "fields/ScalarField.hpp"
#include "equation_systems/LinearEquationSystem.hpp"

std::vector<double> computeResidual(
    const LinearEquationSystem& sys,
    const std::vector<double>& x)
{
    std::vector<double> residual(sys.size(), 0.0);

    for (std::size_t i = 0; i < sys.size(); ++i)
    {
        residual[i] = sys.rhs(i);

        for (auto [j, aij] : sys.row(i))
        {
            residual[i] -= aij * x[j];
        }
    }

    return residual;
}

std::vector<double> computeResidual(
    const LinearEquationSystem& sys,
    const ScalarField& field)
{
    return computeResidual(sys, field.values());
}