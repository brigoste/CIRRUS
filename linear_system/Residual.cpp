#include "linear_system/LinearSystem.hpp"
#include "linear_system/Residual.hpp"

std::vector<double> computeResidual(
    const LinearSystem& sys,
    const std::vector<double>& x)
{
    std::vector<double> r(sys.size(), 0.0);

    const auto& b = sys.RHS();

    const auto n_ = sys.size();

    for (std::size_t i = 0; i < n_; ++i)
    {
        r[i] = b[i];

        for (auto [j, aij] : sys.row(i))
        {
            r[i] -= aij * x[j];
        }
    }

    return r;
}
