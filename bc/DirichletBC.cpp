#include "bc/DirichletBC.hpp"
#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"

DirichletBC::DirichletBC(int i, double T)
    : i_(i), T_(T) {}

void DirichletBC::apply(
    const Mesh1D& mesh,
    Coefficients1D& c
) const
{
    int i = i_;

    // -------------------------------------------------
    // Strong Dirichlet enforcement (row replacement)
    // -------------------------------------------------
    c.aP[i] = 1.0;
    c.aW[i] = 0.0;
    c.aE[i] = 0.0;
    c.b[i]  = T_;
}
