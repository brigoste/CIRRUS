#pragma once

#include "linear_system/LinearSystem.hpp"
#include "mesh/MeshBase.hpp" // for face

class BoundaryCondition
{
public:
    virtual ~BoundaryCondition() = default;

    virtual void apply(
        LinearSystem& sys,
        const Face& face
    ) const = 0;
};
