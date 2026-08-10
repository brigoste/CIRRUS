#pragma once

#include "discretization/interpolators/InterpolationScheme.hpp"

class CentralLinearInterpolation : public InterpolationScheme
{
public:
    InterpolationWeights interpolate( double alpha, double /*flux*/ ) const override;
};