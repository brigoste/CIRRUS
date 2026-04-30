#include "solver/TDMA.hpp"
#include<cmath>
#include<string>
#include<stdexcept>

std::vector<double> TDMA(const Coefficients1D& c) {
    int n = c.aP.size();
    std::vector<double> P(n), Q(n), phi(n);

    P[0] = c.aE[0] / c.aP[0];
    Q[0] = c.b[0]  / c.aP[0];

    for (int i = 1; i < n; i++) {
        double denom = c.aP[i] - c.aW[i] * P[i-1];
        if (std::abs(c.aP[i]) < 1e-14)
            throw std::runtime_error("Zero diagonal at i = " + std::to_string(i));
        if (c.aP[i] <= 0.0) {
            throw std::runtime_error("Non-positive diagonal at i = " + std::to_string(i));
        }
        P[i] = c.aE[i] / denom;
        Q[i] = (c.b[i] + c.aW[i] * Q[i-1]) / denom;
    }

    phi[n-1] = Q[n-1];
    for (int i = n-2; i >= 0; --i)
        phi[i] = P[i]*phi[i+1] + Q[i];

    return phi;
}
