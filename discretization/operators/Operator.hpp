#pragma once

#include "discretization/FluxAccumulator.hpp"
#include "equation_systems/EquationSystem.hpp"

class Operator
{
public:

    virtual ~Operator() = default;

    virtual void assemble(
        const FluxAccumulator& flux,
        EquationSystem& sys
    ) const = 0;
};