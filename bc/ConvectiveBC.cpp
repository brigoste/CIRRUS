#include "ConvectiveBC.hpp"
#include "system/LinearSystem.hpp"
#include "mesh/Mesh1D.hpp"

ConvectiveBC::ConvectiveBC(int face, double h, double Tinf)
    : face_(face), h_(h), Tinf_(Tinf) {}

void ConvectiveBC::apply(const Mesh1D& m, LinearSystem& sys, double k, double A) const
{
    int i = face_;

    double dx = m.dx;

    double a_cond = k * A / dx;
    double a_conv = h_ * A;

    sys.aP[i] = a_cond + a_conv;
    sys.aW[i] = 0.0;
    sys.aE[i] = 0.0;

    sys.b[i] = h_*A*Tinf_;
}
