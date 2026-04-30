#include "discretization/Discretization1D.hpp"

void discretize_1D(
    const Mesh1D& mesh,
    double k,
    double A,
    Coefficients1D& c
) {
    for (int i = 1; i < mesh.n; i++) {
        double aW = k*A/mesh.dx;
        double aE = (i == mesh.n-1 ? 0.0 : k*A/mesh.dx);

        c.aW[i] = aW;
        c.aE[i] = aE;
        c.aP[i] = aW + aE - c.Sp[i];
        c.b[i]  = 0.0 + c.Su[i];
    }
}
