#include "test/verification/ManufacturedCases/Robin1D.hpp"
#include <iostream>

double Robin1D::exact(Point p) const
{
    return 1.0 + p.x[0] + (p.x[0]*p.x[0]);
}

double Robin1D::source(Point p) const
{
    return -k_ * laplacian(p);
}

double Robin1D::laplacian(Point) const
{
    return 2.0;
}

RobinData Robin1D::manufacturedRobinBoundary(const Face& face) const
{
    // Created for a right-boundary robin condition. For left, change dTdx in T_inf to dTdn.
    RobinData robin{};

    double x = face.center.x[0];
    Point p;
    p.x[0] = x;

    const double T    = exact(p);
    const double dTdx = 1.0 + 2.0*x;

    constexpr double h = 10.0;

    robin.transferCoefficient = h;
    robin.referenceValue = T + (k_ * dTdx) / h;     // Backward implmentation. We solve for T_inf from the manufactured solution.

    return robin;
}
