#pragma once

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/MeshBase.hpp"

class GradientScheme
{
public:

    virtual ~GradientScheme() = default;

    virtual void compute(
        const MeshBase& mesh,
        const ScalarField& field,
        VectorField& gradient
    ) const = 0;
};
