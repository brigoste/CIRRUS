#include "GreenGaussGradient.hpp"

void GreenGaussGradient::compute(
    const MeshBase& mesh,
    const ScalarField& field,
    VectorField& gradient
) const
{
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        gradient[c] = Vector();
    }

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const auto& face = mesh.face(f);

        const auto P = face.owner;
        const auto N = face.neighbor;

        double phi_f;

        if (N != Face::INVALID) { phi_f = 0.5 * (field[P] + field[N]); }
        else { phi_f = field[P]; }

        Vector contribution(
            face.normal[0] * phi_f * face.area,
            face.normal[1] * phi_f * face.area,
            face.normal[2] * phi_f * face.area
        );

        gradient[P] += contribution;

        if (N != Face::INVALID) { gradient[N] -= contribution; }
    }

    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        gradient[c] /= mesh.cellVolume(c);
    }
}
