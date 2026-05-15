#include "linear_system/LinearSystem.hpp"
#include "linear_system/Residual.hpp"

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const std::vector<double>& x)
{
    std::vector<double> r(sys.size(), 0.0);

    const auto& b = sys.rhs();

    for (int i = 0; i < sys.size(); ++i)
    {
        r[i] = b[i];

        for (auto [j, aij] : sys.row(i))
        {
            r[i] -= aij * x[j];
        }
    }

    return r;
}
