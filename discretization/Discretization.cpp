#include "discretization/Discretization.hpp"

void discretize(
    const MeshBase& mesh,
    LinearSystem& sys,
    double k
)
{
    const int N = mesh.size();

    for (int p = 0; p < N; ++p)
    {
        double ap = 0.0;

        const auto& nbrs = mesh.neighbors(p);

        for (int q : nbrs)
        {
            double d = mesh.distance(p, q);

            if (d <= 0.0)
                continue;

            // graph-based diffusion coefficient
            double A = mesh.edgeArea(p, q); // must now be "effective interface area"
            double a = k * A / d;

            sys.addCoeff(p, q, a);
            ap += a;
        }

        sys.addDiag(p, ap);

        // NOTE: placeholder physics source term (should be model-driven later)
        // double V = mesh.volume(p);
        sys.addRHS(p, 0.0); // explicit placeholder, no fake physics injection
    }
}
