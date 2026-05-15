#pragma once

#include "BoundaryCondition.hpp"

class NeumannBC : public BoundaryCondition
{
    public:
        NeumannBC(double flux);

        void apply(LinearSystem& sys,
                const Face& face) const override;

    private:
        double flux_;
};
