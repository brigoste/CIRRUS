#pragma once

#include "BoundaryCondition.hpp"

class DirichletBC : public BoundaryCondition {
public:
    DirichletBC(int i, double T);

    void apply(const Mesh1D& mesh,
               Coefficients1D& c) const override;

private:
    int i_;
    double T_;
};
