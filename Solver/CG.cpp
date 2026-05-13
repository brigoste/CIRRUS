#include "Solver/CG.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "linear_system/LinearSystem.hpp"

#include <vector>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>

std::vector<double> CG(
    LinearSystem& sys,
    int iter,
    double tol
)
{
    const int N = sys.size();

    auto& x = sys.solution();
    const auto& b = sys.rhs();

    if ((int)x.size() != N || (int)b.size() != N)
        throw std::runtime_error("CG: size mismatch");

    std::vector<double> r(N, 0.0);
    std::vector<double> p(N, 0.0);
    std::vector<double> Ap(N, 0.0);

    // -----------------------------
    // Initial guess: x = 0
    // -----------------------------
    std::fill(x.begin(), x.end(), 0.0);

    // r = b - A x
    LA::matvec(sys, x, Ap);

    for (int i = 0; i < N; ++i)
    {
        r[i] = b[i] - Ap[i];
        p[i] = r[i];
    }

    double rsold = LA::dot(r, r);

    // -----------------------------
    // CG iteration
    // -----------------------------
    for (int it = 0; it < iter; ++it)
    {
        LA::matvec(sys, p, Ap);

        double pAp = LA::dot(p, Ap);

        if (std::abs(pAp) < 1e-14)
        {
            std::cerr << "CG breakdown: p^T A p ~ 0\n";
            return x;
        }

        double alpha = rsold / pAp;

        LA::axpy(alpha, p, x);
        LA::axpy(-alpha, Ap, r);

        double rsnew = LA::dot(r, r);

        if (std::sqrt(rsnew) < tol)
        {
            std::cout << "CG converged in " << it << " iterations\n";
            return x;
        }

        double beta = rsnew / rsold;

        for (int i = 0; i < N; ++i)
            p[i] = r[i] + beta * p[i];

        rsold = rsnew;
    }

    std::cout << "CG reached max iterations\n";
    return x;
}
