#pragma once

#include "discretization/FaceConvection.hpp"

class MeshBase;
class ScalarField;
class VectorField;

struct ConvectionCoefficients
{
    double owner = 0.0;
    double neighbor = 0.0;
};

class ConvectionScheme
{
public:
    virtual ~ConvectionScheme() = default;

    virtual ConvectionCoefficients coefficients(
        const MeshBase& mesh,
        const FaceConvection& face,
        const ScalarField& field,
        const VectorField& gradient
    ) const = 0;
};