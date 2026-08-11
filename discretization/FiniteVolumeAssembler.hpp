#pragma once

#include "equation_systems/EquationSystem.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "discretization/operators/DiffusionOperator.hpp"
#include "discretization/operators/ConvectionOperator.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/MeshBase.hpp"

class FiniteVolumeAssembler
{
public:
    FiniteVolumeAssembler(
        const ConvectionOperator& convection,
        const DiffusionOperator& diffusion
    )
        : convection_(convection),
        diffusion_(diffusion)
    {}

    void assemble(
        const MeshBase& mesh,
        FluxAccumulator& flux,
        const ScalarField& field,
        const VectorField& gradient,
        EquationSystem& sys
    ) const;

private:
    const ConvectionOperator& convection_;
    const DiffusionOperator& diffusion_;
};
