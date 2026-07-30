#include "tests/verification/ManufacturedCases/Neumann1D.hpp"

double Neumann1D::exact(double x, double) const
{
    return TL_
        + ((-qR_ + volumetricSource_ * L_) / k_) * x
        - (volumetricSource_ / (2.0 * k_)) * x * x;
}

double Neumann1D::source(double, double) const
{
    return volumetricSource_;
}

double Neumann1D::laplacian(double, double) const
{
    return -volumetricSource_ / k_;
}
