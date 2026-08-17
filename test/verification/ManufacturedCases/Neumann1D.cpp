#include "test/verification/ManufacturedCases/Neumann1D.hpp"

double Neumann1D::exact(Point p) const
{
    return TL_
        + ((-qR_ + volumetricSource_ * L_) / k_) * p.x[0]
        - (volumetricSource_ / (2.0 * k_)) * p.x[0]*p.x[0];
}

double Neumann1D::source(Point) const
{
    return volumetricSource_;
}

double Neumann1D::laplacian(Point) const
{
    return -volumetricSource_ / k_;
}
