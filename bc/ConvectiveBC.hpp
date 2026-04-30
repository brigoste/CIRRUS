#pragma once

#include "BoundaryCondition.hpp"

class ConvectiveBC : public BoundaryCondition {
public:
    ConvectiveBC(int i, double h, double Tinf);

    void apply(const Mesh1D& mesh,
               Coefficients1D& c) const override;

private:
    int i_;
    double h_;
    double Tinf_;
};
