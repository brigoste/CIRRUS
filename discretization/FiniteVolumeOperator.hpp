#pragma once

#include "linear_system/LinearSystem.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "discretization/diffusion/DiffusionOperator.hpp"
#include "discretization/operators/ConvectionOperator.hpp"

class FiniteVolumeOperator
{
public:

    FiniteVolumeOperator(
        const ConvectionOperator& convection,
        const DiffusionOperator& diffusion
    )
    :
        convection_(convection),
        diffusion_(diffusion)
    {}

    void assemble(
        const FluxAccumulator& flux,
        LinearSystem& sys
    ) const;

private:

    const ConvectionOperator& convection_;
    const DiffusionOperator& diffusion_;
};