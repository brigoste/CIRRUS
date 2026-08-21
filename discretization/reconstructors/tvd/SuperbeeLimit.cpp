#include "discretization/reconstructors/tvd/SuperbeeLimiter.hpp"

#include <algorithm>

double SuperbeeLimiter::limit(double r) const
{
    return std::max({0.0, 
                     std::min(1.0, 2*r), 
                     std::min(r,2.0)});
}
