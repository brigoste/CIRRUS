#include "solver/GMRES.hpp"

#include "utils/LinearAlgebraUtils.hpp"
#include "utils/Timer.hpp"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <stdexcept>
#include <vector>

std::vector<double> GMRES(
    const LinearEquationSystem& sys,
    int max_iter,
    double tol,
    int restart,
    const Preconditioner& M)
{
    // Timer timer("GMRES total");

    const std::size_t N = sys.size();

    if (restart <= 0)
    {
        throw std::runtime_error( "GMRES: restart must be greater than zero");
    }

    if (max_iter <= 0)
    {
        throw std::runtime_error( "GMRES: max_iter must be greater than zero");
    }

    if (tol <= 0.0)
    {
        throw std::runtime_error( "GMRES: tolerance must be greater than zero");
    }

    if (N == 0)
    {
        return {};
    }

    // ---------------------------------------------------------
    // Initial guess: zero field
    // ---------------------------------------------------------

    std::vector<double> x(N, 0.0);

    // r = b - A*x
    std::vector<double> Ax(N, 0.0);
    std::vector<double> r(N, 0.0);

    sys.matvec(x, Ax);

    for (std::size_t i = 0; i < N; ++i)
    {
        r[i] = sys.rhs(i) - Ax[i];
    }

    const double norm_r0 = LA::norm2(r);

    std::cout
        << "Initial residual = "
        << norm_r0
        << '\n';

    if (norm_r0 < tol)
    {
        std::cout << "Initial guess already satisfies system.\n";

        return x;
    }

    const double convergenceTolerance = tol * norm_r0;

    // ---------------------------------------------------------
    // Krylov basis
    //
    // V[j] contains v_j
    //
    // We need restart + 1 vectors because Arnoldi generates
    // v_{j+1} from v_j.
    // ---------------------------------------------------------

    const int m = std::min(
        restart,
        static_cast<int>(N));

    std::vector<std::vector<double>> V(
        m + 1,
        std::vector<double>(N, 0.0));

    // ---------------------------------------------------------
    // Upper Hessenberg matrix H
    //
    // H has dimensions (m + 1) x m
    // ---------------------------------------------------------

    std::vector<std::vector<double>> H(
        m + 1,
        std::vector<double>(m, 0.0));

    // ---------------------------------------------------------
    // Givens rotation storage
    // ---------------------------------------------------------

    std::vector<double> cs(m, 0.0);
    std::vector<double> sn(m, 0.0);

    // Right-hand side of the small least-squares problem
    std::vector<double> g(m + 1, 0.0);

    std::vector<double> z(N, 0.0);
    std::vector<double> w(N, 0.0);

    int totalIterations = 0;

    // ---------------------------------------------------------
    // Restart cycles
    // ---------------------------------------------------------

    while (totalIterations < max_iter)
    {
        // -----------------------------------------------------
        // Compute current residual
        // -----------------------------------------------------

        sys.matvec(x, Ax);

        for (std::size_t i = 0; i < N; ++i)
        {
            r[i] = sys.rhs(i) - Ax[i];
        }

        const double beta = LA::norm2(r);

        if (beta < convergenceTolerance)
        {
            std::cout
                << "GMRES converged in "
                << totalIterations
                << " iterations. Residual = "
                << beta
                << '\n';

            return x;
        }

        // -----------------------------------------------------
        // Apply preconditioner to residual
        //
        // z = M^-1 r
        // -----------------------------------------------------

        M.apply(r, z);

        const double betaPreconditioned = LA::norm2(z);

        if (betaPreconditioned < 1e-30)
        {
            throw std::runtime_error( "GMRES breakdown: preconditioned residual is zero");
        }

        // -----------------------------------------------------
        // Normalize first Krylov vector
        // -----------------------------------------------------

        for (std::size_t i = 0; i < N; ++i)
        {
            V[0][i] = z[i] / beta;      // betaPreconditioned?
        }

        // Reset Hessenberg matrix and Givens data
        for (auto& row : H)
        {
            std::fill(row.begin(), row.end(), 0.0);
        }

        std::fill(cs.begin(), cs.end(), 0.0);
        std::fill(sn.begin(), sn.end(), 0.0);
        std::fill(g.begin(), g.end(), 0.0);

        g[0] = betaPreconditioned;

        int innerIterations = 0;

        // -----------------------------------------------------
        // Arnoldi process
        // -----------------------------------------------------

        for (int j = 0;
             j < m && totalIterations < max_iter;
             ++j)
        {
            ++totalIterations;
            ++innerIterations;

            // -------------------------------------------------
            // w = A * (M^-1 * V[j])
            // -------------------------------------------------

            M.apply(V[j], z);

            sys.matvec(z, w);

            // -------------------------------------------------
            // Modified Gram-Schmidt
            // -------------------------------------------------

            for (int i = 0; i <= j; ++i)
            {
                H[i][j] = LA::dot(w, V[i]);

                for (std::size_t k = 0; k < N; ++k)
                {
                    w[k] -= H[i][j] * V[i][k];
                }
            }

            H[j + 1][j] = LA::norm2(w);

            // -------------------------------------------------
            // Check for happy breakdown
            // -------------------------------------------------

            if (H[j + 1][j] > 1e-30)
            {
                for (std::size_t k = 0; k < N; ++k)
                {
                    V[j + 1][k] = w[k] / H[j + 1][j];
                }
            }

            // -------------------------------------------------
            // Apply previous Givens rotations
            // -------------------------------------------------

            for (int i = 0; i < j; ++i)
            {
                const double temp = cs[i] * H[i][j] + sn[i] * H[i + 1][j];

                H[i + 1][j] = -sn[i] * H[i][j] + cs[i] * H[i + 1][j];

                H[i][j] = temp;
            }

            // -------------------------------------------------
            // Generate new Givens rotation
            // -------------------------------------------------

            const double h1 = H[j][j];
            const double h2 = H[j + 1][j];

            const double denom = std::hypot(h1, h2);

            if (denom < 1e-30)
            {
                throw std::runtime_error( "GMRES breakdown: zero Hessenberg column");
            }

            const bool happyBreakdown = (std::abs(h2) < 1e-30);

            cs[j] = h1 / denom;
            sn[j] = h2 / denom;

            // -------------------------------------------------
            // Apply Givens rotation to H
            // -------------------------------------------------

            H[j][j] = cs[j] * h1 + sn[j] * h2;

            H[j + 1][j] = 0.0;

            // -------------------------------------------------
            // Apply Givens rotation to g
            // -------------------------------------------------

            const double g1 = g[j];
            const double g2 = g[j + 1];

            g[j] = cs[j] * g1 + sn[j] * g2;

            g[j + 1] = -sn[j] * g1 + cs[j] * g2;

            const double residualEstimate = std::abs(g[j + 1]);

            // -------------------------------------------------
            // Solve/update if converged
            // -------------------------------------------------

            if (residualEstimate < convergenceTolerance || happyBreakdown)
            {
                // ---------------------------------------------
                // Back substitution:
                //
                // H*y = g
                // ---------------------------------------------

                std::vector<double> y( j + 1, 0.0);

                for (int row = j; row >= 0; --row)
                {
                    double sum = g[row];

                    for (int col = row + 1; col <= j; ++col)
                    {
                        sum -= H[row][col] * y[col];
                    }

                    if (std::abs(H[row][row]) < 1e-30)
                    {
                        throw std::runtime_error( "GMRES breakdown: singular Hessenberg system");
                    }

                    y[row] = sum / H[row][row];
                }

                // ---------------------------------------------
                // x = x + M^-1(V*y)
                // ---------------------------------------------

                std::vector<double> correction(N, 0.0);

                for (int col = 0; col <= j; ++col)
                {
                    for (std::size_t k = 0; k < N; ++k)
                    {
                        correction[k] += y[col] * V[col][k];
                    }
                }

                M.apply(correction, z);

                for (std::size_t i = 0; i < N; ++i)
                {
                    x[i] += z[i];
                }

                // ---------------------------------------------
                // Compute true residual
                // ---------------------------------------------

                sys.matvec(x, Ax);

                for (std::size_t k = 0; k < N; ++k)
                {
                    r[k] = sys.rhs(k) - Ax[k];
                }

                const double trueResidual = LA::norm2(r);

                if (trueResidual < convergenceTolerance)
                {
                    std::cout
                        << "GMRES converged in "
                        << totalIterations
                        << " iterations. Residual = "
                        << trueResidual
                        << '\n';

                    return x;
                }

                if (happyBreakdown)
                {
                    throw std::runtime_error("GMRES breakdown: Krylov space terminated before convergence");
                }

                break;
            }
        }

        // -----------------------------------------------------
        // Restart update
        //
        // If we reach here without convergence, solve the
        // current least-squares problem and update x before
        // constructing the next Krylov space.
        // -----------------------------------------------------

        const int k = innerIterations;

        if (k == 0)
        {
            break;
        }

        std::vector<double> y(k, 0.0);

        for (int row = k - 1; row >= 0; --row)
        {
            double sum = g[row];

            for (int col = row + 1; col < k; ++col)
            {
                sum -= H[row][col] * y[col];
            }

            if (std::abs(H[row][row]) < 1e-30)
            {
                throw std::runtime_error( "GMRES breakdown: singular Hessenberg system");
            }

            y[row] = sum / H[row][row];
        }

        // -----------------------------------------------------
        // Form correction in Krylov space
        // -----------------------------------------------------

        std::vector<double> correction(N, 0.0);

        for (int col = 0; col < k; ++col)
        {
            for (std::size_t i = 0; i < N; ++i)
            {
                correction[i] += y[col] * V[col][i];
            }
        }

        // -----------------------------------------------------
        // Apply preconditioner
        // -----------------------------------------------------

        // x += correction;
        M.apply(correction, z);

        for (std::size_t i = 0; i < N; ++i)
        {
            x[i] += z[i];
        }

        // -----------------------------------------------------
        // Check true residual after restart
        // -----------------------------------------------------

        sys.matvec(x, Ax);

        for (std::size_t i = 0; i < N; ++i)
        {
            r[i] = sys.rhs(i) - Ax[i];
        }

        const double trueResidual = LA::norm2(r);

        if (trueResidual < convergenceTolerance)
        {
            std::cout
                << "GMRES converged in "
                << totalIterations
                << " iterations. Residual = "
                << trueResidual
                << '\n';

            return x;
        }
    }

    // ---------------------------------------------------------
    // Failed to converge
    // ---------------------------------------------------------

    sys.matvec(x, Ax);

    for (std::size_t i = 0; i < N; ++i)
    {
        r[i] = sys.rhs(i) - Ax[i];
    }

    const double finalResidual = LA::norm2(r);

    std::cerr
        << "[WARN] GMRES failed to converge after "
        << totalIterations
        << " iterations. Residual = "
        << finalResidual
        << '\n';

    return x;
}