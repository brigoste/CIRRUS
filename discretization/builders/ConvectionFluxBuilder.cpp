#include "discretization/builders/ConvectionFluxBuilder.hpp"

void ConvectionFluxBuilder::apply(
    const MeshBase& mesh,
    const PhysicsModel& model,
    FluxAccumulator& flux
) const
{
    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        if (N != Face::INVALID)
        {
            const double F = model.convectionFaceFlux(face);

            flux.addConvection(P, N, f, F);
        }
    }
}
