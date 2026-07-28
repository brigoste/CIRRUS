#include "tests/verification/ManufacturedCases/Sinusoidal2D.hpp"
#include "utils/MathConstants.hpp"

double Sinusoidal2D::exact(
    double x,
    double y) const
{
    return std::sin(math::PI * x) *
           std::sin(math::PI * y);
}

double Sinusoidal2D::source(double x, double y) const
{
    return -k_ * laplacian(x, y);
}

double Sinusoidal2D::laplacian(
    double x,
    double y) const
{
    return -2.0 * math::PI * math::PI *
           std::sin(math::PI * x) *
           std::sin(math::PI * y);
}
