#include "solver/CG.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "linear_system/LinearSystem.hpp"

#include <vector>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>

static std::vector<double> matVec(const LinearSystem sys, const std::vector<double>& x)
{
    const std::size_t N = sys.size();
    std::vector<double> y(N, 0.0);

    for (std::size_t i = 0; i < N; ++i)
    {
        const auto& row = sys.row(i);

        for (const auto& [j, aij] : row) { y[i] += aij * x[j]; }
    }

    return y;
}

std::vector<double> CG(
    const LinearSystem& sys,
    int max_iter,
    double tol)
{
    const std::size_t N = sys.size();

    std::vector<double> x(N, 0.0);
    std::vector<double> r(N), p(N), Ap(N);

    // r = b - A x
    Ap = matVec(sys, x);

    for (std::size_t i = 0; i < N; ++i)
    {
        r[i] = sys.rhs(i) - Ap[i];
        p[i] = r[i];
    }

    double rs_old = 0.0;
    for (double v : r) rs_old += v * v;

    for (int iter = 0; iter < max_iter; ++iter)
    {
        Ap = matVec(sys, p);

        double alpha_num = rs_old;
        double alpha_den = 0.0;

        for (std::size_t i = 0; i < N; ++i) { alpha_den += p[i] * Ap[i]; }

        if (std::abs(alpha_den) < 1e-14) { break; }

        double alpha = alpha_num / alpha_den;

        double rs_new = 0.0;

        for (std::size_t i = 0; i < N; ++i)
        {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
            rs_new += r[i] * r[i];
        }

        if (std::sqrt(rs_new) < tol) { break; }

        for (std::size_t i = 0; i < N; ++i) { p[i] = r[i] + (rs_new / rs_old) * p[i]; }

        rs_old = rs_new;
    }

    return x;
}
