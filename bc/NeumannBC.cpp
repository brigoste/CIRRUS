#include "bc/NeumannBC.hpp"

NeumannBC::NeumannBC(double flux)
    : flux_(flux)
{}

void NeumannBC::apply(
    LinearSystem& sys,
    const BoundaryContext& ctx) const
{
    int i = ctx.owner;

    double A = ctx.area;

    // flux enters RHS as source term
    sys.addRHS(i, flux_ * A);
}
