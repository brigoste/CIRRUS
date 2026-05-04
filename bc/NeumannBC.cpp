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
    int N = mesh.n;  

    double k  = mesh.k;
    double A  = mesh.A;
    double dx = mesh.dx;

    double diff = k * A / dx;  // face conductivity

    // Left boundary (i = 0)
    if (i == 0)
    {
        c.aP[i] = diff;
        c.aW[i] = 0.0;
        c.aE[i] = diff;
        c.b[i] += q_ * A;
    }
    // Right boundary (i = N-1)
    else if (i == N - 1)
    {
        c.aP[i] = diff;
        c.aE[i] = 0.0;
        c.aW[i] = diff;
        c.b[i] += q_ * A;
    }
    else
    {
        throw std::runtime_error(
            "NeumannBC can only be applied to boundary nodes (0 or N-1)."
        );
    }
}
