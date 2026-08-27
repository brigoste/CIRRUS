#include "discretization/builders/SourceFluxBuilder.hpp"
#include "mesh/MeshBase.hpp"
#include "physics/ScalarTransport/ScalarTransportModel.hpp"
#include "test/verification/VerificationCase.hpp"

void SourceFluxBuilder::apply(
    const MeshBase& mesh,
    const ScalarTransportModel& model,
    FluxAccumulator& flux,
    const VerificationCase* verificationCase
) const
{
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const double source = model.cellSource(mesh, c);
        // Physical model source
        flux.addSource({
            c,
            source * mesh.cellVolume(c),
            0.0
        });

        // Manufactured verification forcing
        if (verificationCase)
        {
            const auto& xc = mesh.cellCenter(c);

            const double manufacturedSource = verificationCase->source(xc);

            flux.addSource({
                c,
                manufacturedSource * mesh.cellVolume(c),
                0.0
            });
        }
    }
}
