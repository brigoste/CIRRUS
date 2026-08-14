#include "MinmodLimiter.hpp"

#include <algorithm>

double MinmodLimiter::limit(double r) const
{
    return std::max(0.0, std::min(1.0, r));
}