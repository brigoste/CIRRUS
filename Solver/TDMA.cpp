#include "Solver/TDMA.hpp"
#include <iostream>

#include <vector>
#include <stdexcept>

// Solve A x = b using TDMA (Thomas algorithm)
// REQUIREMENT: A is strictly tri-diagonal.
std::vector<double> TDMA(const LinearSystem& sys)
{
    const std::size_t n = sys.size();
    if (n == 0) { return {}; }

    // Extract tridiagonal coefficients
    std::vector<double> a(n, 0.0); // sub-diagonal  (i,i-1)
    std::vector<double> b(n, 0.0); // diagonal      (i,i)
    std::vector<double> c(n, 0.0); // super         (i,i+1)
    std::vector<double> d = sys.RHS();  // copy RHS

    for (std::size_t i = 0; i < n; ++i)
    {
        const auto& row = sys.row(i);

        // Diagonal must exist
        auto itDiag = row.find(i);
        if (itDiag == row.end()) { throw std::runtime_error("TDMA: missing diagonal at row " + std::to_string(i)); }
        b[i] = itDiag->second;

        // Off-diagonals (if present)
        if (i > 0) {
            auto itL = row.find(i - 1);
            if (itL != row.end()) { a[i] = itL->second; }
        }

        if (i + 1 < n) {
            auto itU = row.find(i + 1);
            if (itU != row.end()) { c[i] = itU->second; }
        }

        // Check for illegal entries (non-zero outside tri-diagonal)
        for (auto& kv : row)
        {
            std::size_t j = kv.first;
            if (j + 1 < i || j > i + 1)  // outside band
            {
                if (std::abs(kv.second) > 1e-14) { throw std::runtime_error( "TDMA: matrix row " + std::to_string(i) + " contains non-tridiagonal entry at column " + std::to_string(j)); }
            }
        }
    }

    // --------------------------------------------------------
    // Forward sweep
    // --------------------------------------------------------
    for (std::size_t i = 1; i < n; ++i)
    {
        if (std::abs(b[i - 1]) < 1e-20) { throw std::runtime_error("TDMA: zero pivot at row " + std::to_string(i - 1)); }

        const double m = a[i] / b[i - 1];
        b[i] -= m * c[i - 1];
        d[i] -= m * d[i - 1];
    }

    // --------------------------------------------------------
    // Back substitution
    // --------------------------------------------------------
    std::vector<double> x(n, 0.0);

    if (std::abs(b[n - 1]) < 1e-20) { throw std::runtime_error("TDMA: zero final pivot at row n-1"); }

    x[n - 1] = d[n - 1] / b[n - 1];

    for (int i = static_cast<int>(n) - 2; i >= 0; --i)
    {
        double denom = b[i];
        if (std::abs(denom) < 1e-20) { throw std::runtime_error("TDMA: zero pivot at row " + std::to_string(i)); }

        x[i] = (d[i] - c[i] * x[i + 1]) / denom;
    }

    return x;
}
