#pragma once

#include "discretization/FaceConvection.hpp"
#include "equation_systems/EquationSystem.hpp"

class ConvectionScheme
{
public:

    virtual ~ConvectionScheme() = default;

    virtual void assemble(
        const FaceConvection& face,
        EquationSystem& sys
    ) const = 0;
};