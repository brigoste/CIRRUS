#pragma once

#include "BoundaryCondition.hpp"

class ConvectiveBC : public BoundaryCondition
{
    public:
        ConvectiveBC(double h, double Tinf);

        void apply(LinearSystem& sys,
                const Face& face) const override;

    private:
        double h_;
        double Tinf_;
};
