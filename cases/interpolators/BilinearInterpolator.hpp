#pragma once

#include "interpolators/Interpolator.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include <stdexcept>

class BilinearInterpolator : public Interpolator
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

    double interpolate2D(
        const ScalarField& field,
        const Point& position
    ) const;

    Vector interpolate2D(
        const VectorField& field,
        const Point& position
    ) const;

    double interpolate2DFace(
        const ScalarField& field,
        const Point& position
    ) const;

    Vector interpolate2DFace(
        const VectorField& field,
        const Point& position
    ) const;
};
