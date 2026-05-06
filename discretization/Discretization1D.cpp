#include "discretization/Discretization1D.hpp"

void discretize_1D(
    const Mesh1D& mesh,
    LinearSystem& sys,
    double k,
    double A
)
{
    double dx = mesh.dx;
    double a = k * A / dx;

    for (int i = 1; i < mesh.n - 1; i++)
    {
        sys.aW[i] = -a;
        sys.aE[i] = -a;
        sys.aP[i] = a + a - sys.Sp[i];
        sys.b[i]  = sys.Su[i];
    }
}
