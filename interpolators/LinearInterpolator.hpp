#pragma once

#include "interpolators/Interpolator.hpp"
#include "fields/ScalarField.hpp"

class LinearInterpolator : public Interpolator
{
public:
    double interpolate(
        const ScalarField& field,
        const Point& position
    ) const override;
};
