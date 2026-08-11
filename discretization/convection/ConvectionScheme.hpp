#pragma once

#include "discretization/FaceConvection.hpp"

class ConvectionScheme
{
public:
    virtual ~ConvectionScheme() = default;

    virtual double faceCoefficient( const FaceConvection& face ) const = 0;
};
