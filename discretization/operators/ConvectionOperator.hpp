#pragma once

#include "discretization/operators/Operator.hpp"
#include "discretization/reconstructors/ReconstructionScheme.hpp"
#include "mesh/MeshBase.hpp"

class EquationSystem;
class FluxAccumulator;
class ScalarField;
class VectorField;

class ConvectionOperator
{
public:
    explicit ConvectionOperator(
        const ReconstructionScheme& reconstruction);

    void assemble(
        const MeshBase& mesh,
        FluxAccumulator& flux,
        const ScalarField& field,
        const VectorField& gradient
    ) const;

private:
    const ReconstructionScheme& reconstruction_;
};
