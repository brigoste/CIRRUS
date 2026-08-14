#include "discretization/builders/DiffusionFluxBuilder.hpp"

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "physics/PhysicsModel.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "test/verification/VerificationCase.hpp"
#include <stdexcept>
#include <iostream>

void DiffusionFluxBuilder::apply(
    const MeshBase& mesh,
    const PhysicsModel& model,
    const BoundaryPatchSystem& boundary,
    FluxAccumulator& flux,
    const VerificationCase* verificationCase
) const
{
    if (flux.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "DiffusionFluxBuilder: FluxAccumulator size mismatch"
        );
    }

    // =====================================================
    // 1. INTERIOR FACES
    // =====================================================
    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        const std::size_t P = face.owner;
        const std::size_t N = face.neighbor;

        if (N != Face::INVALID)
        {
            const double D = model.diffusionFaceCoefficient(face);

            FaceDiffusion contribution({ P, N, f, D });
            flux.addDiffusion(contribution);
        }
    }

    // =====================================================
    // 2. BOUNDARY FACES
    // =====================================================
    for (std::size_t g = 0; g < mesh.nBoundaryGroups(); ++g)
    {
        const auto* bc = boundary.getGroup(g);

        if (!bc)
        {
            continue;
        }

        const auto& faces = mesh.boundaryFaces(g);

        for (std::size_t f : faces)
        {
            const Face& face = mesh.face(f);
            const std::size_t P = face.owner;

            const double D = model.diffusionFaceCoefficient(face);

            switch (bc->type)
            {
                case bc::Type::Dirichlet:
                {
                    double value = bc->value;

                    if (verificationCase) { value = verificationCase->exact( face.center.x[0], face.center.x[1] ); }

                    BoundaryDiffusion contribution({P, Face::INVALID, f, D, value});
                    flux.addBoundaryDiffusion( contribution );
                    
                    break;
                }

                case bc::Type::Neumann:
                {
                    double manufacturedBoundaryFlux = bc->flux;

                    if (verificationCase) { manufacturedBoundaryFlux = verificationCase->manufacturedBoundaryFlux(face); }

                    flux.addSource( {P, -manufacturedBoundaryFlux * face.area, 0.0} );

                    break;
                }

                case bc::Type::Robin:
                {
                    double transferCoefficient = bc->transferCoefficient;
                    double referenceValue = bc->referenceValue;

                    if (verificationCase)
                    {
                        RobinData robin = verificationCase->manufacturedRobinBoundary(face);

                        transferCoefficient = robin.transferCoefficient;
                        referenceValue = robin.referenceValue;
                    }

                    const double hA = transferCoefficient * face.area;
                    const double H = (hA * D) / (hA + D);

                    flux.addSource({P, H * referenceValue, -H});

                    break;
                }

                default:
                    break;
            }
        }
    }
}
