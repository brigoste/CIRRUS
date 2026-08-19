#pragma once

#include "interpolators/Interpolator.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

class LinearInterpolator : public Interpolator
{
public:

    double interpolate(
        const ScalarField& field,
        const Point& position
    ) const override;

    Vector interpolate(
        const VectorField& field,
        const Point& position
    ) const override;

private:

    double interpolate1D(
        const ScalarField& field,
        const Point& position
    ) const;

    Vector interpolate1D(
        const VectorField& field,
        const Point& position
    ) const;

    double interpolate2D(
        const ScalarField& field,
        const Point& position
    ) const;

    Vector interpolate2D(
        const VectorField& field,
        const Point& position
    ) const;
};
