#pragma once

#include "discretization/FaceDiffusion.hpp"

class FluxAccumulator;

class DiffusionScheme
{
public:
    virtual ~DiffusionScheme() = default;

    virtual void assemble(
        const FaceDiffusion& face,
        FluxAccumulator& flux
    ) const = 0;
};
