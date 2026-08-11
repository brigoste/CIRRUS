#pragma once

#include "discretization/operators/Operator.hpp"
#include "discretization/convection/ConvectionScheme.hpp"
#include "mesh/MeshBase.hpp"

class EquationSystem;
class FluxAccumulator;
class ScalarField;
class VectorField;

class ConvectionOperator
{
public:
    explicit ConvectionOperator(const ConvectionScheme& scheme);

    void assemble(
        const MeshBase& mesh,
        const FluxAccumulator& flux,
        const ScalarField& field,
        const VectorField& gradient,
        EquationSystem& sys
    ) const;

private:
    const ConvectionScheme& scheme_;
};