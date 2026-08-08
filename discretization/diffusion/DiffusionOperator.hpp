#pragma once

#include "discretization/operators/Operator.hpp"
#include "discretization/builders/DiffusionFluxBuilder.hpp"

class DiffusionOperator : public Operator
{
public:

    void assemble(
        const FluxAccumulator& flux,
        LinearSystem& sys
    ) const override;
};