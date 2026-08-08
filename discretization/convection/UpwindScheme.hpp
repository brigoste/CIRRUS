#pragma once

#include "discretization/convection/ConvectionScheme.hpp"

class UpwindScheme : public ConvectionScheme
{
public:

    void assemble(
        const FaceConvection& face,
        EquationSystem& sys
    ) const override;
};