#pragma once

#include "mesh/Mesh1D.hpp"
#include "system/LinearSystem.hpp"

class BoundaryCondition {
public:
    virtual ~BoundaryCondition() = default;

    virtual void apply(const Mesh1D& mesh,
                       LinearSystem& sys,
                       double k,
                       double A) const = 0;
};
