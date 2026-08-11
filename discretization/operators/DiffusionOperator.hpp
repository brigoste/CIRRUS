#pragma once

#include "discretization/operators/Operator.hpp"
#include "discretization/diffusion/DiffusionScheme.hpp"

class DiffusionOperator : public Operator
{
public:

    explicit DiffusionOperator(
        const DiffusionScheme& scheme
    );

    void assemble(
        const MeshBase& mesh,
        FluxAccumulator& flux
    ) const override;

private:

    const DiffusionScheme& scheme_;
};
