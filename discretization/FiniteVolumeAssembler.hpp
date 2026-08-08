#pragma once

#include "equation_systems/EquationSystem.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "discretization/diffusion/DiffusionOperator.hpp"
#include "discretization/operators/ConvectionOperator.hpp"

class FiniteVolumeAssembler
{
public:

    FiniteVolumeAssembler(
        const ConvectionOperator& convection,
        const DiffusionOperator& diffusion
    )
    :
        convection_(convection),
        diffusion_(diffusion)
    {}

    void assemble(
        const FluxAccumulator& flux,
        EquationSystem& sys
    ) const;

private:

    const ConvectionOperator& convection_;
    const DiffusionOperator& diffusion_;
};