#pragma once

#include "discretization/interpolators/InterpolationScheme.hpp"

class UpwindInterpolation : public InterpolationScheme
{
public:
    InterpolationWeights interpolate( double alpha, double flux ) const override;
};