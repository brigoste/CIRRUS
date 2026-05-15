#include "discretization/Discretization.hpp"
#include <limits>

// static constexpr std::size_t INVALID =
//     std::numeric_limits<std::size_t>::max();

void discretize(
    const MeshBase& mesh,
    LinearSystem& sys,
    double k)
{
    const std::size_t C = mesh.ncells();

    // ---------------------------------------------------------
    // FACE-BASED DIFFUSION (FV STANDARD)
    // ---------------------------------------------------------
    for (std::size_t f = 0; f < mesh.nFaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        const double d = face.centroidDistance;
        const double D = k * face.area / d;

        // interior face
        if (N != INVALID)
        {
            sys.addCoeff(P, P,  D);
            sys.addCoeff(P, N, -D);

            sys.addCoeff(N, N,  D);
            sys.addCoeff(N, P, -D);
        }
        else
        {
            // boundary face
            const BoundaryConditionDescriptor& bc = face.bc;

            if (bc.type == BCType::Dirichlet)
            {
                sys.addCoeff(P, P, D);
                sys.addRHS(P, 2.0 * D * bc.value);
            }
            else if (bc.type == BCType::Neumann)
            {
                sys.addRHS(P, bc.flux * face.area);
            }
            else if (bc.type == BCType::Convective)
            {
                const double hA = bc.h * face.area;
                sys.addCoeff(P, P, hA);
                sys.addRHS(P, hA * bc.Tinf);
            }
        }
    }

    // ---------------------------------------------------------
    // SOURCE TERMS (cell-centered placeholder)
    // ---------------------------------------------------------
    for (std::size_t c = 0; c < C; ++c)
    {
        sys.addRHS(c, 0.0);
    }
}
