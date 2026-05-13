#include "Solver/SOR.hpp"
#include "linear_system/LinearSystem.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <algorithm>

std::vector<double> SOR(
    LinearSystem& sys,
    int iter,
    double tol,
    double omega,
    bool output
)
{
    const int N = sys.size();

    auto& x = sys.solution();
    const auto& b = sys.rhs();

    if ((int)x.size() != N || (int)b.size() != N)
        throw std::runtime_error("SOR: size mismatch");

    std::fill(x.begin(), x.end(), 0.0);

    for (int it = 0; it < iter; ++it)
    {
        double maxDiff = 0.0;

        for (int i = 0; i < N; ++i)
        {
            const double diag = sys.diag(i);

            if (std::abs(diag) < 1e-14)
                throw std::runtime_error("SOR: zero diagonal");

            double sum = 0.0;

            // graph-based off-diagonal sum
            for (const auto& [j, aij] : sys.row(i))
            {
                sum += aij * x[j];
            }

            const double gs = (b[i] - sum) / diag;

            const double x_new =
                (1.0 - omega) * x[i] + omega * gs;

            maxDiff = std::max(maxDiff, std::abs(x_new - x[i]));
            x[i] = x_new;
        }

        if (output)
            std::cout << "SOR iter " << it
                      << " maxDiff=" << maxDiff << "\n";

        if (maxDiff < tol)
        {
            if (output)
                std::cout << "SOR converged in " << it << " iterations\n";
            return x;
        }
    }

    if (output)
        std::cout << "SOR reached max iterations\n";

    return x;
}
