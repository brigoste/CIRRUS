#pragma once

#include "BoundaryCondition.hpp"

class DirichletBC : public BoundaryCondition
{
    public:
        DirichletBC(double value);

        void apply(LinearSystem& sys,
                const BoundaryContext& ctx) const override;

    private:
        double value_;
};
