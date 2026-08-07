#pragma once

#include "discretization/convection/ConvectionScheme.hpp"

class CentralDifferenceScheme : public ConvectionScheme
{
public:

    void assemble(
        const FaceConvection& face,
        LinearSystem& sys
    ) const override;
};