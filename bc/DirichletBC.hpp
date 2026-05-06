#pragma once
#include "BoundaryCondition.hpp"

class DirichletBC : public BoundaryCondition {
    public:
        DirichletBC(int face, double value);

        void apply(const Mesh1D& m, LinearSystem& sys, double k, double A) const override;

    private:
        int face_;
        double value_;
};
