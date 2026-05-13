#include "bc/ConvectiveBC.hpp"

ConvectiveBC::ConvectiveBC(double h, double Tinf)
    : h_(h), Tinf_(Tinf)
{}

void ConvectiveBC::apply(
    LinearSystem& sys,
    const BoundaryContext& ctx) const
{
    int i = ctx.owner;

    double A = ctx.area;

    // Robin formulation:
    // adds h*A to diagonal
    sys.addDiag(i, h_ * A);

    // adds h*A*Tinf to RHS
    sys.addRHS(i, h_ * A * Tinf_);
}
