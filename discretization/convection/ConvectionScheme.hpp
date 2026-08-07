#pragma once

#include "discretization/FaceConvection.hpp"
#include "linear_system/LinearSystem.hpp"

class ConvectionScheme
{
public:

    virtual ~ConvectionScheme() = default;

    virtual void assemble(
        const FaceConvection& face,
        LinearSystem& sys
    ) const = 0;
};