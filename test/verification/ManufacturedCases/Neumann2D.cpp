#include "test/verification/ManufacturedCases/Neumann2D.hpp"

double Neumann2D::exact(double x, double y) const
{
    return T0_
         + a_*x
         + b_*y
         + c_*x*x
         + d_*y*y;
}

double Neumann2D::source(double x, double y) const
{
    return -k_ * laplacian(x,y);
}

double Neumann2D::laplacian(double, double) const
{
    return 2.0*c_ + 2.0*d_;
}

double Neumann2D::boundaryFlux(const Face& face) const
{
    const double x = face.center.x[0];
    const double y = face.center.x[1];

    const double dTdx = a_ + 2.0*c_*x;
    const double dTdy = b_ + 2.0*d_*y;

    double qn = -k_ * ( dTdx*face.normal.x[0] + dTdy*face.normal.x[1] );

    return qn;
}
