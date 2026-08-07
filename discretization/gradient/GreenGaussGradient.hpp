#pragma once

#include "discretization/gradient/GradientScheme.hpp"

class GreenGaussGradient : public GradientScheme
{
public:

    void compute(
        const MeshBase& mesh,
        const ScalarField& field,
        VectorField& gradient
    ) const override;
};