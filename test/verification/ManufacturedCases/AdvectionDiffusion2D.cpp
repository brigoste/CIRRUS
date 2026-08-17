#include "AdvectionDiffusion2D.hpp"
#include <cmath>

double AdvectionDiffusion2D::exact(Point p) const
{
    return std::sin(math::PI * p.x[0]) * std::sin(math::PI * p.x[1]);
}

double AdvectionDiffusion2D::laplacian(Point p) const
{
    const double pi2 = math::PI * math::PI;
    return -2.0 * pi2 * exact(p);
}
double AdvectionDiffusion2D::source(Point p) const
{
    const double phi_x = math::PI * std::cos(math::PI * p.x[0]) * std::sin(math::PI * p.x[1]);

    const double phi_y = math::PI * std::sin(math::PI * p.x[0]) * std::cos(math::PI * p.x[1]);

    const double lap = laplacian(p);

    return rho_ * (ux_ * phi_x + uy_ * phi_y) - gamma_ * lap;
}
