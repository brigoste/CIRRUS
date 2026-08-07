#include "LeastSquaresGradient.hpp"

#include "mesh/primitives/Face.hpp"

#include <cmath>
#include <stdexcept>

void LeastSquaresGradient::compute(
    const MeshBase& mesh,
    const ScalarField& field,
    VectorField& gradient
) const
{
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const Point& xP = mesh.cellCenter(c);
        const double phiP = field[c];

        //
        // Normal equation terms:
        //
        // [sum(dx^2)   sum(dxdy)] [gx] = [sum(dx*dphi)]
        // [sum(dxdy)   sum(dy^2)] [gy]   [sum(dy*dphi)]
        //

        double a = 0.0; // dx^2
        double b = 0.0; // dx*dy
        double d = 0.0; // dy^2

        double rhs_x = 0.0;
        double rhs_y = 0.0;

        const Cell& cell = mesh.cell(c);

        for (std::size_t faceIndex : cell.faces)
        {
            const Face& face = mesh.face(faceIndex);

            std::size_t neighbor;

            if (face.owner == c)
            {
                neighbor = face.neighbor;
            }
            else
            {
                neighbor = face.owner;
            }

            // Boundary face
            if (neighbor == Face::INVALID)
            {
                continue;
            }

            const Point& xN = mesh.cellCenter(neighbor);

            const double dx = xN[0] - xP[0];
            const double dy = xN[1] - xP[1];

            const double dphi = field[neighbor] - phiP;

            a += dx * dx;
            b += dx * dy;
            d += dy * dy;

            rhs_x += dx * dphi;
            rhs_y += dy * dphi;
        }

        const double determinant = a * d - b * b;

        if (std::abs(determinant) < 1e-14)
        {
            throw std::runtime_error(
                "LeastSquaresGradient: singular matrix"
            );
        }

        const double gx =
            (d * rhs_x - b * rhs_y)
            / determinant;

        const double gy =
            (a * rhs_y - b * rhs_x)
            / determinant;

        gradient[c] = Vector(gx, gy, 0.0);
    }
}