#include "test/verification/ManufacturedCases/Neumann2D.hpp"

double Neumann2D::exact(Point p) const
{
    return T0_
         + a_*p.x[0]
         + b_*p.x[1]
         + c_*(p.x[0]*p.x[0])
         + d_*(p.x[1]*p.x[1]);
}

double Neumann2D::source(Point p) const
{
    return -k_ * laplacian(p);
}

double Neumann2D::laplacian(Point) const
{
    return 2.0*c_ + 2.0*d_;
}

double Neumann2D::manufacturedBoundaryFlux(const Face& face) const
{
    const double x = face.center.x[0];
    const double y = face.center.x[1];

    const double dTdx = a_ + 2.0*c_*x;
    const double dTdy = b_ + 2.0*d_*y;

    double qn = -k_ * ( dTdx*face.normal.x + dTdy*face.normal.y );

    return qn;
}
