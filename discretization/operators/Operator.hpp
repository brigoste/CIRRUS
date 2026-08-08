#pragma once

#include "discretization/FluxAccumulator.hpp"
#include "linear_system/LinearSystem.hpp"

class Operator
{
public:

    virtual ~Operator() = default;

    virtual void assemble(
        const FluxAccumulator& flux,
        LinearSystem& sys
    ) const = 0;
};