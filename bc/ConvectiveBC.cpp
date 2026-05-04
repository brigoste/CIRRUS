#include "ConvectiveBC.hpp"
#include "mesh/Mesh1D.hpp"
#include "coeffs/Coefficients1D.hpp"
#include <stdexcept>

ConvectiveBC::ConvectiveBC(int i, double h, double Tinf)
    : i_(i), h_(h), Tinf_(Tinf) {}

void ConvectiveBC::apply(
    const Mesh1D& mesh,
    Coefficients1D& c
) const
{
    int i = i_;
    int N = mesh.n;

    double k  = mesh.k;
    double A  = mesh.A;
    double dx = mesh.dx;

    double diff = k * A / dx;   // conduction coefficient
    double beta = h_ * A;       // convection coefficient

    // -----------------------------------
    // LEFT boundary (i = 0)
    // -----------------------------------
    if (i == 0)
    {
        c.aP[i] = diff + beta;
        c.aW[i] = 0.0;
        c.aE[i] = diff;
        c.b[i] += beta * Tinf_;
    }
    // -----------------------------------
    // RIGHT boundary (i = N-1)
    // -----------------------------------
    else if (i == N - 1)
    {
        c.aP[i] = diff + beta;
        c.aE[i] = 0.0;
        c.aW[i] = diff;
        c.b[i] += beta * Tinf_;
    }
    else
    {
        throw std::runtime_error(
            "ConvectiveBC must be applied only on boundaries (i=0 or i=N-1)."
        );
    }
}
