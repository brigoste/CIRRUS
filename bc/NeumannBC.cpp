#include "NeumannBC.hpp"
#include "system/LinearSystem.hpp"
#include "mesh/Mesh1D.hpp"

NeumannBC::NeumannBC(int face, double q)
    : face_(face), q_(q) {}

void NeumannBC::apply(const Mesh1D& m, LinearSystem& sys, double k, double A) const
{
    int i = face_;
    double dx = m.dx; 

    // Neumann:  -k dT/dx = q
    // dT/dx ≈ (T_i - T_im1)/dx

    double flux_term = q_ * A * dx / k;

    sys.b[i] += flux_term;
    // sys.aW[i] = -1.0;
    // sys.aP[i] =  1.0;
}
