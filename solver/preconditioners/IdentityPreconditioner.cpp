#include "solver/preconditioners/IdentityPreconditioner.hpp"

void IdentityPreconditioner::setup(
    const LinearEquationSystem&)
{
    // Nothing to do.
}

void IdentityPreconditioner::apply(
    const std::vector<double>& r,
    std::vector<double>& z) const
{
    z = r;
}
