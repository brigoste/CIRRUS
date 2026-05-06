#pragma once
#include "BoundaryCondition.hpp"
#include "mesh/Mesh1D.hpp"

class NeumannBC : public BoundaryCondition {
    public:
        NeumannBC(int face, double q);

        void apply(const Mesh1D& m, LinearSystem& sys, double k, double A) const override;

    private:
        int face_;
        double q_;
};
