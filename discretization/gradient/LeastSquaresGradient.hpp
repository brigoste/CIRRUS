#pragma once

#include "discretization/gradient/GradientScheme.hpp"

class LeastSquaresGradient : public GradientScheme
{
public:

    void compute(
        const MeshBase& mesh,
        const ScalarField& field,
        VectorField& gradient
    ) const override;
};