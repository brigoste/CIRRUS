#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Vector.hpp"

class ScalarField;
class VectorField;

class Interpolator
{
public:
    virtual ~Interpolator() = default;

    virtual double interpolate(
        const ScalarField& field,
        const Point& position
    ) const = 0;

    virtual Vector interpolate(
        const VectorField& field,
        const Point& position
    ) const = 0;
};
