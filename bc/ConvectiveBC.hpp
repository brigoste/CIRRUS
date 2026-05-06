#pragma once
#include "BoundaryCondition.hpp"

class ConvectiveBC : public BoundaryCondition {
    public:
        ConvectiveBC(int face, double h, double Tinf);

        void apply(const Mesh1D& m, LinearSystem& sys, double k, double A) const override;

    private:
        int face_;
        double h_;
        double Tinf_;
};
