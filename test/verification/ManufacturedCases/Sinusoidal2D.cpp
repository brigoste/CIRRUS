#include "test/verification/ManufacturedCases/Sinusoidal2D.hpp"
#include "utils/MathConstants.hpp"

double Sinusoidal2D::exact(
    Point p) const
{
    return std::sin(math::PI * p.x[0]) *
           std::sin(math::PI * p.x[1]);
}

double Sinusoidal2D::source(Point p) const
{
    return -k_ * laplacian(p);
}

double Sinusoidal2D::laplacian(
    Point p) const
{
    return -2.0 * math::PI * math::PI *
           std::sin(math::PI * p.x[0]) *
           std::sin(math::PI * p.x[1]);
}
