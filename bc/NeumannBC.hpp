#pragma once

#include "BoundaryCondition.hpp"

class NeumannBC : public BoundaryCondition {
public:
    NeumannBC(int i, double q);

    void apply(const Mesh1D& mesh,
               Coefficients1D& c) const override;

private:
    int i_;
    double q_;
};
