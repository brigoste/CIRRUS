#pragma once

#include "linear_system/LinearSystem.hpp"
#include "bc/BoundaryContext.hpp"

class BoundaryCondition
{
public:
    virtual ~BoundaryCondition() = default;

    virtual void apply(
        LinearSystem& sys,
        const BoundaryContext& ctx
    ) const = 0;
};
