#include "TDMA.hpp"
#include <vector>
#include <stdexcept>
#include <cmath>
#include <iostream>

std::vector<double> TDMA(const LinearSystem& sys)
{
    const int n = sys.aP.size();

    std::vector<double> c(n, 0.0);
    std::vector<double> d(n, 0.0);
    std::vector<double> x(n, 0.0);

    // boundary already enforced in sys
    c[0] = sys.aE[0] / sys.aP[0];
    d[0] = sys.b[0]  / sys.aP[0];

    for (int i = 1; i < n; ++i)
    {
        double denom = sys.aP[i] - sys.aW[i] * c[i - 1];

        c[i] = sys.aE[i] / denom;
        d[i] = (sys.b[i] - sys.aW[i] * d[i - 1]) / denom;
    }

    x[n - 1] = d[n - 1];

    for (int i = n - 2; i >= 0; --i)
    {
        x[i] = d[i] - c[i] * x[i + 1];
    }

    return x;
}
