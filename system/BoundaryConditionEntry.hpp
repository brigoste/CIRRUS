#pragma once

#include <memory>
#include "bc/BoundaryCondition.hpp"
#include "bc/BoundaryFace.hpp"

struct BoundaryConditionEntry
{
    BoundaryFace face;

    std::unique_ptr<BoundaryCondition> bc;

    BoundaryConditionEntry(BoundaryFace f, std::unique_ptr<BoundaryCondition> b)
        : face(f), bc(std::move(b)) {}
};
