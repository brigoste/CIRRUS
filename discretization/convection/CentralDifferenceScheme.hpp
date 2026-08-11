#pragma once

#include "discretization/convection/ConvectionScheme.hpp"

class CentralDifferenceScheme : public ConvectionScheme
{
public:
    double faceCoefficient(
        const FaceConvection& face
    ) const override;
};
