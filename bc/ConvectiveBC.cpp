#include "bc/ConvectiveBC.hpp"
#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"

ConvectiveBC::ConvectiveBC(int i, double h, double Tinf)
    : i_(i), h_(h), Tinf_(Tinf) {}

void ConvectiveBC::apply(
    const Mesh1D& mesh,
    Coefficients1D& c
) const
{
    int i = i_;

    double k  = mesh.k;
    double A  = mesh.A;
    double dx = mesh.dx;

    // -------------------------------------------------
    // Diffusion contribution at boundary face
    // -------------------------------------------------
    double diff = k * A / dx;

    // Convection contribution
    double hA = h_ * A;

    // -------------------------------------------------
    // Robin (convective) boundary condition
    // -------------------------------------------------
    c.aP[i] = diff + hA;     // MUST guarantee non-zero diagonal
    c.aW[i] = 0.0;
    c.aE[i] = 0.0;

    c.b[i]  = hA * Tinf_;
}
