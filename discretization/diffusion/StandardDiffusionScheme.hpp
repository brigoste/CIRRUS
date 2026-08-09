#pragma once

#include "discretization/diffusion/DiffusionScheme.hpp"

class StandardDiffusionScheme : public DiffusionScheme
{
public:

    void assemble(
        const FaceDiffusion& face,
        EquationSystem& sys
    ) const override;
};