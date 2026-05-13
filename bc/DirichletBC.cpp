#include "bc/DirichletBC.hpp"

DirichletBC::DirichletBC(double value)
    : value_(value)
{}

void DirichletBC::apply(
    LinearSystem& sys,
    const BoundaryContext& ctx) const
{
    int i = ctx.node;

    sys.clearRow(i);

    // enforce u = value
    sys.addDiag(i, 1.0);
    sys.setRHS(i, value_);
}
