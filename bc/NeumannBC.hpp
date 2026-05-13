#pragma once

#include "BoundaryCondition.hpp"

class NeumannBC : public BoundaryCondition
{
    public:
        NeumannBC(double flux);

        void apply(LinearSystem& sys,
                const BoundaryContext& ctx) const override;

    private:
        double flux_;
};
