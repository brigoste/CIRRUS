#pragma once

#include "BoundaryCondition.hpp"

class ConvectiveBC : public BoundaryCondition
{
    public:
        ConvectiveBC(double h, double Tinf);

        void apply(LinearSystem& sys,
                const BoundaryContext& ctx) const override;

    private:
        double h_;
        double Tinf_;
};
