#include "discretization/SourceFluxBuilder.hpp"
#include "mesh/MeshBase.hpp"
#include "physics/PhysicsModel.hpp"
#include "test/verification/VerificationCase.hpp"

void SourceFluxBuilder::apply(
    const MeshBase& mesh,
    const PhysicsModel& model,
    FluxAccumulator& flux,
    const VerificationCase* verificationCase
) const
{
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const double source = model.cellSource(mesh, c);
        // Physical model source
        flux.addSource(
            c,
            source * mesh.cellVolume(c),
            0.0
        );

        // Manufactured verification forcing
        if (verificationCase)
        {
            const auto& xc = mesh.cellCenter(c);

            const double manufacturedSource =
                verificationCase->source(
                    xc.x[0],
                    xc.x[1]
                );

            flux.addSource(
                c,
                manufacturedSource * mesh.cellVolume(c),
                0.0
            );
        }
    }
}