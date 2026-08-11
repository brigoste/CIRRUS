#pragma once

#include "discretization/convection/ConvectionScheme.hpp"

class UpwindScheme : public ConvectionScheme
{
public:
    double faceCoefficient(
        const FaceConvection& face
    ) const override;
};
