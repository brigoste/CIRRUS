#include "Solver/TDMA.hpp"
#include <iostream>

std::vector<double> TDMA(
    LinearSystem& sys,
    int N,
    bool output
)
{
    std::vector<double> a(N), b(N), c(N), d(N);
    std::vector<double> c_star(N), d_star(N);

    // -----------------------------
    // Extract tridiagonal system
    // -----------------------------
    for (int i = 0; i < N; ++i)
    {
        const double diag = sys.diagonal()[i];
        // const auto& row = sys.row(i);

        d[i] = sys.rhs()[i];
        b[i] = diag;

        a[i] = (i > 0)     ? sys.coeff(i, i - 1) : 0.0;
        c[i] = (i < N - 1) ? sys.coeff(i, i + 1) : 0.0;
    }

    // -----------------------------
    // Forward sweep
    // -----------------------------
    c_star[0] = c[0] / b[0];
    d_star[0] = d[0] / b[0];

    for (int i = 1; i < N; ++i)
    {
        double m = 1.0 / (b[i] - a[i] * c_star[i - 1]);
        c_star[i] = (i < N - 1) ? c[i] * m : 0.0;
        d_star[i] = (d[i] - a[i] * d_star[i - 1]) * m;
    }

    // -----------------------------
    // Back substitution
    // -----------------------------
    std::vector<double> x(N);

    x[N - 1] = d_star[N - 1];

    for (int i = N - 2; i >= 0; --i)
        x[i] = d_star[i] - c_star[i] * x[i + 1];

    sys.setSolution(x);

    if (output)
        std::cout << "TDMA solved 1D system of size " << N << "\n";

    return x;
}
