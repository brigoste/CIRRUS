#include "solver/SOR.hpp"
#include "equation_systems/LinearEquationSystem.hpp"

#include <cmath>
#include <stdexcept>
#include <string>
#include <algorithm>
#include <vector>

std::vector<double> SOR(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    double omega)
{
    const std::size_t N = sys.size();
    std::vector<double> x(N, 0.0);

    for (int iter = 0; iter < max_iter; ++iter)
    {
        double maxRes = 0.0;

        for (std::size_t i = 0; i < N; ++i)
        {
            const auto& row = sys.row(i);

            double diag = 0.0;
            double sum = 0.0;

            for (const auto& [j, aij] : row)
            {
                if (j == i) { diag = aij; }
                else { sum += aij * x[j]; }
            }

            if (std::abs(diag) < 1e-14) { throw std::runtime_error("Zero diagonal at row " + std::to_string(i)); }

            double x_gs = (sys.rhs(i) - sum) / diag;
            double x_new = (1.0 - omega) * x[i] + omega * x_gs;

            maxRes = std::max(maxRes, std::abs(x_new - x[i]));
            x[i] = x_new;
        }

        if (!std::isfinite(maxRes)) { throw std::runtime_error("Non-finite residual in SOR"); }

        if (maxRes < tol) { break; }
    }

    return x;
}
