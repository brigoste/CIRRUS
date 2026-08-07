#include "discretization/diffusion/DiffusionOperator.hpp"

#include "mesh/primitives/Face.hpp"

#include <stdexcept>

void DiffusionOperator::apply(
    const MeshBase& mesh,
    const PhysicsModel& model,
    FluxAccumulator& flux
) const
{
    if (flux.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "DiffusionOperator: FluxAccumulator size mismatch"
        );
    }

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        // -------------------------------------------------
        // Interior face
        // -------------------------------------------------
        if (N != Face::INVALID)
        {
            const double D =
                model.diffusionFaceCoefficient(face);

            flux.addDiffusion(P, N, D);
        }
    }
}