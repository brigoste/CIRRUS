#include "discretization/interpolators/UpwindInterpolation.hpp"

InterpolationWeights UpwindInterpolation::interpolate(
double /*alpha*/,
double flux
) const
{
    if (flux >= 0.0)
    {
        // Flow from owner -> neighbor.
        return { 1.0, 0.0 };
    }

    // Flow from neighbor -> owner.
    return { 0.0, 1.0 };
}