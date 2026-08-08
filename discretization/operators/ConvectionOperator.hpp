#pragma once

#include "discretization/operators/Operator.hpp"
#include "discretization/convection/ConvectionScheme.hpp"

class ConvectionOperator : public Operator
{
public:

    explicit ConvectionOperator(
        const ConvectionScheme& scheme
    );

    void assemble(
        const FluxAccumulator& flux,
        LinearSystem& sys
    ) const override;

private:

    const ConvectionScheme& scheme_;
};