#include "discretization/interpolators/CentralLinearInterpolation.hpp"

InterpolationWeights CentralLinearInterpolation::interpolate( double alpha, double /*flux*/ ) const
{
    return { 1.0 - alpha, alpha };
}