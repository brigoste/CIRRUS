#include "solver/BiCGSTAB.hpp"
#include "solver/preconditioners/Preconditioner.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "utils/Timer.hpp"

#include <stdexcept>
#include <iostream>

std::vector<double> BiCGSTAB(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    const Preconditioner& M)
{
    // Timer timer("BiCGSTAB total");

    const std::size_t N = sys.size();

    // -----------------------------
    // Initial guess: zero field
    // -----------------------------
    std::vector<double> x(N, 0.0);
    std::vector<double> r(N), r0_hat(N);
    std::vector<double> v(N, 0.0);
    std::vector<double> p(N, 0.0);
    std::vector<double> t(N);
    std::vector<double> p_hat(N);
    std::vector<double> s_hat(N);
    std::vector<double> s(N);

    // r = b - A x
    sys.matvec(x, r);

    for (std::size_t i = 0; i < N; ++i)
    {
        r[i] = sys.rhs(i) - r[i];
    }

    std::cout << "Initial residual = " << LA::norm2(r) << '\n';

    r0_hat = r;

    double rho_old = 1.0;
    double alpha    = 1.0;
    double omega    = 1.0;

    double norm_r0 = LA::norm2(r);
    if (norm_r0 < tol) 
    {
        std::cout << "Initial guess already satisfies system.\n";
        return x;
    }

    double resid = norm_r0;

    for (int iter = 0; iter < max_iter; ++iter) 
    {
        double rho_new = LA::dot(r0_hat, r);

        // std::cout << "iter " << iter << " rho = " << rho_new << '\n';

        if (std::abs(rho_new) < 1e-30) 
        { 
            throw std::runtime_error("BiCGSTAB breakdown: rho ~ 0"); 
        }

        if (iter == 0) 
        { 
            p = r; 
        }
        else 
        {
            double beta = (rho_new / rho_old) * (alpha / omega);

            for (std::size_t i = 0; i < N; ++i) 
            {
                p[i] = r[i] + beta * (p[i] - omega * v[i]);
            }
        }

        // Apply preconditioner M to p
        M.apply(p, p_hat);

        // v = A(M⁻¹p)
        sys.matvec(p_hat, v);

        const double r0v = LA::dot(r0_hat, v);
        if (std::abs(r0v) < 1e-30) 
        { 
            throw std::runtime_error("BiCGSTAB breakdown: <r0_hat,v> ~ 0"); 
        }

        alpha = rho_new / r0v;

        // s = r - alpha v
        for (std::size_t i = 0; i < N; ++i) 
        {
            s[i] = r[i] - alpha * v[i];
        }

        // early convergence check
        double norm_s = LA::norm2(s);
        if (norm_s < tol * norm_r0) 
        {
            for (std::size_t i = 0; i < N; ++i) 
            { 
                x[i] += alpha * p_hat[i]; 
            }

            std::cout << "BiCGSTAB converged in " << iter << " iterations. Residual = " << norm_s << "\n";
            return x;
        }

        // Apply preconditioner to s
        M.apply(s, s_hat);

        // t = A(M⁻¹s)
        sys.matvec(s_hat, t);
        const double tt = LA::dot(t, t);
        if (std::abs(tt) < 1e-30) 
        { 
            throw std::runtime_error("BiCGSTAB breakdown: <t,t> ~ 0"); 
        }

        omega = LA::dot(t, s) / tt;
        if (std::abs(omega) < 1e-30) 
        { 
            throw std::runtime_error("BiCGSTAB breakdown: omega ~ 0"); 
        }

        // update x
        for (std::size_t i = 0; i < N; ++i) 
        {
            x[i] += alpha * p_hat[i] + omega * s_hat[i];
        }

        // update r
        for (std::size_t i = 0; i < N; ++i) 
        {
            r[i] = s[i] - omega * t[i];
        }

        resid = LA::norm2(r);

        if (resid < tol * norm_r0) 
        { 
            std::cout << "BiCGSTAB converged in " << iter << " iterations. Residual = " << resid << "\n";
            return x; 
        }

        rho_old = rho_new;

        // if (iter % 100 == 0) { std::cout << "BiCGSTAB iter " << iter << ": residual = " << resid << "\n"; }
    }

    std::cerr << "[WARN] BiCGSTAB failed to converge after " << max_iter << " iterations. Residual = " << resid << "\n";

    return x;
}
