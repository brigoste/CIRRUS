#pragma once

#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"

class BoundaryCondition {
public:
    virtual ~BoundaryCondition() = default;

    virtual void apply(const Mesh1D& mesh,
                       Coefficients1D& c) const = 0;
};
