#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Point.hpp"

class ScalarField;

class Interpolator
{
public:
    virtual ~Interpolator() = default;

    virtual double interpolate(
        const ScalarField& field,
        const Point& position
    ) const = 0;
};
