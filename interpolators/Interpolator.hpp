#pragma once

#include "fields/ScalarField.hpp"
#include "mesh/primitives/Point.hpp"

class Interpolator
{
public:
    virtual ~Interpolator() = default;

    virtual double interpolate(
        const ScalarField& field,
        const Point& position
    ) const = 0;
};