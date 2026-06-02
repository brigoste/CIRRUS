#include "discretization/Discretization.hpp"

void discretize(
    const MeshBase& mesh,
    LinearSystem& sys,
    double k)
{
    const std::size_t Nf = mesh.nfaces();
    const std::size_t Nc = mesh.ncells();

    sys.resize(static_cast<int>(Nc));

    for (std::size_t f = 0; f < Nf; ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        const double D = k * face.area / face.d;

        // --------------------------
        // Interior face
        // --------------------------
        if (N != Face::INVALID)
        {
            sys.addCoeff(P, P,  D);
            sys.addCoeff(P, N, -D);

            sys.addCoeff(N, N,  D);
            sys.addCoeff(N, P, -D);
        }
        // --------------------------
        // Boundary face
        // --------------------------
        else
        {
            // intentionally empty:
            // handled in BC layer
        }
    }
}
