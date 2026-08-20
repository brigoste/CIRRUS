#include "discretization/reconstructors/tvd/VanLeerLimiter.hpp"

#include <algorithm>

double VanLeerLimiter::limit(double r) const
{
    return (r + std::abs(r)) / (1.0 + std::abs(r));
}
