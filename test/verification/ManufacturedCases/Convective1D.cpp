#include "test/verification/ManufacturedCases/Convective1D.hpp"
#include <iostream>

double Convective1D::exact(double x, double) const
{
    return 1.0 + x + x*x;
}

double Convective1D::source(double x, double) const
{
    return -k_ * laplacian(x, 0.0);
}

double Convective1D::laplacian(double, double) const
{
    return 2.0;
}

ConvectiveData Convective1D::manufacturedRobinBoundary(const Face& face) const
{
    // Created for a right-boundary robin condition. For left, change dTdx in T_inf to dTdn.
    ConvectiveData robin{};

    const double x = face.center.x[0];

    const double T    = exact(x, 0.0);
    const double dTdx = 1.0 + 2.0*x;

    constexpr double h = 10.0;

    robin.transferCoefficient = h;
    robin.referenceValue = T + (k_ * dTdx) / h;     // Backward implmentation. We solve for T_inf from the manufactured solution.

    return robin;
}
