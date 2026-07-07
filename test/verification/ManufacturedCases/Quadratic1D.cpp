#include "tests/verification/ManufacturedCases/Quadratic1D.hpp"

double Quadratic1D::exact( double x, double) const
{
    return x*x + (TR - TL - 1)*x + TL;
}

double Quadratic1D::source(double x, double y) const
{
    return -k_ * laplacian(x, y);
}

double Quadratic1D::laplacian( double, double) const
{
    return 2.0;      // -2*k
}
