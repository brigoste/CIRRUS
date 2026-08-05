#include "solver/preconditioners/IdentityPreconditioner.hpp"


void IdentityPreconditioner::setup(
    const LinearSystem&)
{
    // Nothing to do.
}


void IdentityPreconditioner::apply(
    const std::vector<double>& r,
    std::vector<double>& z) const
{
    z = r;
}
