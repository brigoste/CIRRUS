#pragma once

#include "discretization/convection/ConvectionScheme.hpp"

class UpwindScheme : public ConvectionScheme
{
public:
    ConvectionCoefficients coefficients(
    const MeshBase& mesh,
    const FaceConvection& face,
    const ScalarField& field,
    const VectorField& gradient
    ) const override;
};