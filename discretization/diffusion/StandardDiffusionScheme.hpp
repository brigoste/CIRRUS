#pragma once

#include "discretization/diffusion/DiffusionScheme.hpp"

class FluxAccumulator;

class StandardDiffusionScheme : public DiffusionScheme
{
public:

    void assemble(
        const FaceDiffusion& face,
        FluxAccumulator& flux
    ) const override;
};
