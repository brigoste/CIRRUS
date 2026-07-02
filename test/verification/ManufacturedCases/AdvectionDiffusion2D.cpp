#include "AdvectionDiffusion2D.hpp"
#include <cmath>

double AdvectionDiffusion2D::exact(double x, double y) const
{
    return std::sin(math::PI * x) * std::sin(math::PI * y);
}

double AdvectionDiffusion2D::laplacian(double x, double y) const
{
    const double pi2 = math::PI * math::PI;
    return -2.0 * pi2 * exact(x, y);
}
double AdvectionDiffusion2D::source(double x, double y) const
{
    const double phi_x = math::PI * std::cos(math::PI * x) * std::sin(math::PI * y);

    const double phi_y = math::PI * std::sin(math::PI * x) * std::cos(math::PI * y);

    const double lap = laplacian(x, y);

    return rho_ * (ux_ * phi_x + uy_ * phi_y) - gamma_ * lap;
}
