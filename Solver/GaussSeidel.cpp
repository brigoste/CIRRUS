#include "Solver/GaussSeidel.hpp"
#include "linear_system/LinearSystem.hpp"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <algorithm>

std::vector<double> GaussSeidel(
    LinearSystem& sys,
    int iter,
    double tol,
    bool output
)
{
    const int N = sys.size();

    auto& x = sys.solution();
    const auto& b = sys.rhs();

    if ((int)x.size() != N || (int)b.size() != N)
        throw std::runtime_error("GaussSeidel: size mismatch");

    std::fill(x.begin(), x.end(), 0.0);

    for (int it = 0; it < iter; ++it)
    {
        double maxDiff = 0.0;

        for (int i = 0; i < N; ++i)
        {
            const double diag = sys.diag(i);

            if (std::abs(diag) < 1e-14)
                throw std::runtime_error("GaussSeidel: zero diagonal");

            double sum = 0.0;

            // off-diagonal contributions
            for (const auto& [j, aij] : sys.row(i))
            {
                sum += aij * x[j];
            }

            const double x_new = (b[i] - sum) / diag;

            maxDiff = std::max(maxDiff, std::abs(x_new - x[i]));
            x[i] = x_new;
        }

        if (output)
            std::cout << "GS iter " << it
                      << " maxDiff=" << maxDiff << "\n";

        if (maxDiff < tol)
        {
            if (output)
                std::cout << "GS converged in " << it << " iterations\n";
            return x;
        }
    }

    if (output)
        std::cout << "GS reached max iterations\n";

    return x;
}
