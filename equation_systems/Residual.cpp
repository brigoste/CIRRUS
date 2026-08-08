#include "equation_systems/Residual.hpp"

#include "fields/ScalarField.hpp"
#include "equation_systems/LinearSystem.hpp"

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const std::vector<double>& x)
{
    std::vector<double> residual(sys.size(), 0.0);

    const auto& rhs = sys.RHS();

    for (std::size_t i = 0; i < sys.size(); ++i)
    {
        residual[i] = rhs[i];

        for (auto [j, aij] : sys.row(i))
        {
            residual[i] -= aij * x[j];
        }
    }

    return residual;
}

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const ScalarField& field)
{
    return computeResidual(sys, field.values());
}
