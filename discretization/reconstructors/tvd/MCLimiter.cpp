#include "discretization/reconstructors/tvd/MCLimiter.hpp"

#include <algorithm>

double MCLimiter::limit(double r) const
{
    return std::max(
        0.0,
        std::min(
            {
                2.0 * r,
                0.5 * (1.0 + r),
                2.0
            }
        )
    );
}
