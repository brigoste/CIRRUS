#include "NeumannBC.hpp"
#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"

NeumannBC::NeumannBC(int i, double q)
    : i_(i), q_(q) {}

void NeumannBC::apply(
    const Mesh1D& mesh,
    Coefficients1D& c
) const
{
    int i = i_;

    double k  = mesh.k;
    double A  = mesh.A;
    double dx = mesh.dx;

    // -------------------------------------------------
    // diffusion contribution at boundary face
    // -------------------------------------------------
    double diff = k * A / dx;

    // -------------------------------------------------
    // enforce Neumann flux weakly
    // -------------------------------------------------
    c.aP[i] = diff;              // MUST guarantee non-zero diagonal
    c.aW[i] = 0.0;
    c.aE[i] = 0.0;

    c.b[i] += q_ * A;            // heat flux into domain
}
