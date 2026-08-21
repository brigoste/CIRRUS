#include "discretization/reconstructors/tvd/VanAlbadaLimiter.hpp"

#include <algorithm>

double VanAlbadaLimiter::limit(double r) const
{
    if (r <= 0.0) { return 0.0; }
    return (r * r + r) / (r * r + 1.0);
}
