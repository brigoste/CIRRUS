#include "solver/TDMA.hpp"

#include <iostream>
#include <vector>
#include <stdexcept>
#include <cmath>


// Solve A x = b using TDMA (Thomas algorithm)
// REQUIREMENT: A is strictly tri-diagonal.

std::vector<double> TDMA(const LinearSystem& sys)
{
    const std::size_t n = sys.size();

    if (n == 0)
    {
        return {};
    }

    // Extract tridiagonal coefficients
    std::vector<double> a(n, 0.0); // sub-diagonal
    std::vector<double> b(n, 0.0); // diagonal
    std::vector<double> c(n, 0.0); // super-diagonal
    std::vector<double> d = sys.RHS();


    for (std::size_t i = 0; i < n; ++i)
    {
        const auto& row = sys.row(i);


        for (const auto& [j,value] : row)
        {
            // diagonal
            if (j == i) { b[i] = value; }

            // sub-diagonal
            else if (j + 1 == i) { a[i] = value; }

            // super-diagonal
            else if (j == i + 1) { c[i] = value; }

            // illegal entry
            else
            {
                if (std::abs(value) > 1e-14)
                {
                    throw std::runtime_error(
                        "TDMA: matrix row "
                        + std::to_string(i)
                        + " contains non-tridiagonal entry at column "
                        + std::to_string(j)
                    );
                }
            }
        }


        if (std::abs(b[i]) < 1e-20)
        {
            throw std::runtime_error(
                "TDMA: missing or zero diagonal at row "
                + std::to_string(i)
            );
        }
    }


    // --------------------------------------------------------
    // Forward sweep
    // --------------------------------------------------------

    for (std::size_t i = 1; i < n; ++i)
    {
        if (std::abs(b[i-1]) < 1e-20) { throw std::runtime_error( "TDMA: zero pivot at row " + std::to_string(i-1) ); }

        const double m = a[i] / b[i-1];

        b[i] -= m*c[i-1];
        d[i] -= m*d[i-1];
    }


    // --------------------------------------------------------
    // Back substitution
    // --------------------------------------------------------

    std::vector<double> x(n,0.0);


    if (std::abs(b[n-1]) < 1e-20) { throw std::runtime_error( "TDMA: zero final pivot" ); }


    x[n-1] = d[n-1]/b[n-1];


    for (int i = static_cast<int>(n)-2; i >= 0; --i)
    {
        if (std::abs(b[i]) < 1e-20) { throw std::runtime_error( "TDMA: zero pivot at row " + std::to_string(i) ); }

        x[i] = (d[i] - c[i]*x[i+1]) / b[i];
    }


    return x;
}
