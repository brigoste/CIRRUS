#include "solver/BiCGSTAB.hpp"
#include "utils/LinearAlgebraUtils.hpp"

#include <stdexcept>
#include <iostream>

std::vector<double> BiCGSTAB(
    const LinearSystem& sys,
    int max_iter,
    double tol)
{
    const std::size_t N = sys.size();

    if (sys.RHS().size() != N)
        throw std::runtime_error("BiCGSTAB: RHS size mismatch");

    // -----------------------------
    // Initial guess: zero field
    // -----------------------------
    std::vector<double> x(N, 0.0);
    std::vector<double> r(N), r0_hat(N);
    std::vector<double> v(N, 0.0);
    std::vector<double> p(N, 0.0);
    std::vector<double> t(N);

    // r = b - A x
    LA::residual(sys, x, r);
    r0_hat = r;

    double rho_old = 1.0;
    double alpha    = 1.0;
    double omega    = 1.0;

    double norm_r0 = LA::norm2(r);
    if (norm_r0 == 0.0)
        return x;

    double resid = norm_r0;

    for (int iter = 0; iter < max_iter; ++iter)
    {
        double rho_new = LA::dot(r0_hat, r);

        if (std::abs(rho_new) < 1e-30)
            throw std::runtime_error("BiCGSTAB breakdown: rho ~ 0");

        if (iter == 0)
        {
            p = r;
        }
        else
        {
            double beta =
                (rho_new / rho_old) *
                (alpha / omega);

            for (std::size_t i = 0; i < N; ++i)
            {
                p[i] = r[i] + beta * (p[i] - omega * v[i]);
            }
        }

        // v = A p
        LA::matvec(sys, p, v);

        const double r0v = LA::dot(r0_hat, v);
        if (std::abs(r0v) < 1e-30)
            throw std::runtime_error("BiCGSTAB breakdown: <r0_hat,v> ~ 0");

        alpha = rho_new / r0v;

        // s = r - alpha v
        std::vector<double> s = r;
        for (std::size_t i = 0; i < N; ++i)
            s[i] -= alpha * v[i];

        // early convergence check
        double norm_s = LA::norm2(s);
        if (norm_s < tol * norm_r0)
        {
            for (std::size_t i = 0; i < N; ++i)
                x[i] += alpha * p[i];

            return x;
        }

        // t = A s
        LA::matvec(sys, s, t);
        const double tt = LA::dot(t, t);
        if (std::abs(tt) < 1e-30)
            throw std::runtime_error("BiCGSTAB breakdown: <t,t> ~ 0");

        omega = LA::dot(t, s) / tt;

        // update x
        for (std::size_t i = 0; i < N; ++i)
        {
            x[i] += alpha * p[i] + omega * s[i];
        }

        // update r
        for (std::size_t i = 0; i < N; ++i)
        {
            r[i] = s[i] - omega * t[i];
        }

        resid = LA::norm2(r);

        if (resid < tol * norm_r0)
            return x;

        if (std::abs(omega) < 1e-30)
            throw std::runtime_error("BiCGSTAB breakdown: omega ~ 0");

        rho_old = rho_new;
    }

    std::cerr << "[WARN] BiCGSTAB did not fully converge. Residual = "
              << resid << "\n";

    return x;
}
