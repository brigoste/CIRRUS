#include "test/verification/ManufacturedCases/Quadratic1D.hpp"
#include <iostream>

double Quadratic1D::exact(Point p) const
{
    return TL_ 
           + ((TR_ - TL_) / L_ + volumetricSource_ * L_ / (2.0 * k_)) * p.x[0] 
           - (volumetricSource_ / (2.0*k_)) * p.x[0] * p.x[0];
}

double Quadratic1D::source(Point) const
{
    return volumetricSource_;
}

double Quadratic1D::laplacian(Point) const
{
    return -volumetricSource_/k_;
}
/*
    Manufactured solution:

        -k d2T/dx2 = q'''

    therefore:

        d2T/dx2 = -q'''/k

    The source term remains positive; the Laplacian carries
    the negative sign from the governing equation.
*/
