#include "solver/CG.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "linear_system/LinearSystem.hpp"

#include "utils/Timer.hpp"

#include <vector>
#include <iostream>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <chrono>
#include <limits>

static std::vector<double> matVec(const LinearSystem& sys, const std::vector<double>& x)
{
    const std::size_t N = sys.size();
    std::vector<double> y(N, 0.0);

    static bool printed = false;

    if (!printed)
    {
        std::cout
            << "Matrix size: "
            << N
            << " x "
            << N
            << "\n";

        std::cout
            << "NNZ: "
            << sys.nnz()
            << "\n";

        std::cout
            << "NNZ/row: "
            << static_cast<double>(sys.nnz()) / N
            << "\n";

        printed = true;
    }

    for (std::size_t i = 0; i < N; ++i)
    {
        const auto& row = sys.row(i);

        for (const auto& [j, aij] : row) 
        { 
            y[i] += aij * x[j]; 
        }
    }

    return y;
}

static void printDiagonalStats(const LinearSystem& sys)
{
    double minDiag = std::numeric_limits<double>::max();
    double maxDiag = 0.0;

    for (std::size_t i = 0; i < sys.size(); ++i)
    {
        double d = std::abs(sys.diagonal(i));

        minDiag = std::min(minDiag, d);
        maxDiag = std::max(maxDiag, d);
    }

    std::cout << "Diagonal range: "
              << minDiag
              << " - "
              << maxDiag
              << "\n";
}

std::vector<double> CG(
    const LinearSystem& sys,
    int max_iter,
    double tol,
    const Preconditioner& M)
{
    //Timer timer("CG total");
    const std::size_t N = sys.size();

    std::vector<double> x(N, 0.0);
    std::vector<double> r(N), p(N), Ap(N);

    // r = b - A x
    Ap = matVec(sys, x);

    // Initialization

    std::vector<double> z(N);       // Preconditioning

    for(std::size_t i = 0; i < N; ++i) 
    { 
        r[i] = sys.rhs(i) - Ap[i]; 
    }

    static bool printed_diag = false;

    if (!printed_diag)
    {
        printDiagonalStats(sys);

        printed_diag = true;
    }

    M.apply(r,z);

    p = z;

    double rz_old = 0.0;

    for (std::size_t i = 0; i < N; ++i) 
    { 
        rz_old += r[i] * z[i]; 
    }

    double matVecTime = 0.0;
    int iter_count = 0;

    for (int iter = 0; iter < max_iter; ++iter)
    {
        if (iter == 0)
        {
            double rz = 0.0;
            for (std::size_t i = 0; i < N; ++i)
            {
                rz += r[i] * z[i];
            }

            std::cout << "Initial r.z = " << rz << "\n";
        }
        ++iter_count;

        auto start = std::chrono::high_resolution_clock::now();

        Ap = matVec(sys, p);

        auto end = std::chrono::high_resolution_clock::now();

        matVecTime += std::chrono::duration<double>(end-start).count();

        double alpha_num = rz_old;
        double alpha_den = 0.0;

        for (std::size_t i = 0; i < N; ++i) 
        { 
            alpha_den += p[i] * Ap[i]; 
        }

        if (std::abs(alpha_den) < 1e-14) 
        { 
            break; 
        }

        double alpha = alpha_num / alpha_den;

        for (std::size_t i = 0; i < N; ++i)
        {
            x[i] += alpha * p[i];
            r[i] -= alpha * Ap[i];
        }

        double residual = 0.0;

        for (double v : r) 
        { 
            residual += v*v; 
        }

        residual = std::sqrt(residual);

        if (residual < tol)
        {
            std::cout << "CG converged. Residual = " << residual << "\n";
            break;
        }

        M.apply(r,z);

        double rz_new = 0.0;

        for (std::size_t i = 0; i < N; ++i) 
        { 
            rz_new += r[i] * z[i]; 
        }

        // double preconditioned_residual = std::sqrt(rz_new);

        // if (iter % 100 == 0 || residual < tol) { std::cout << "CG iter " << iter << ": " << "||r|| = " << residual << ", " << "sqrt(rz) = " << std::sqrt(rz_new) << "\n"; }

        double beta = rz_new / rz_old;

        for (std::size_t i = 0; i < N; ++i) 
        { 
            p[i] = z[i] + beta * p[i]; 
        }

        rz_old = rz_new;
    }

    std::cout << "CG iterations: " << iter_count << "\n";
    std::cout << "CG matVec total: " << matVecTime << " s\n";
    std::cout << "Average matVec: " << matVecTime / iter_count << " s\n";

    return x;
}
