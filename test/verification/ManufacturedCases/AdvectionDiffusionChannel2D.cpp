#include "test/verification/ManufacturedCases/AdvectionDiffusionChannel2D.hpp"
#include <cmath>

double AdvectionDiffusionChannel2D::exact(Point p) const
{
    (void) p;
    return 400.0;
}

double AdvectionDiffusionChannel2D::laplacian(Point p) const
{
    (void) p;
    return 0.0;
}
double AdvectionDiffusionChannel2D::source(Point p) const
{
    (void) p;
    return 0.0;
}
double AdvectionDiffusionChannel2D::manufacturedBoundaryFlux( const Face& face) const
{
    (void) face;
    return 0.0;
}