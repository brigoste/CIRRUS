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

            flux.addDiffusion(P, N, D);
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

                    if (verificationCase)
                    {
                        value = verificationCase->exact( face.center.x[0], face.center.x[1] );
                    }

                    flux.addBoundaryDiffusion( P, D, value );
                    
                    break;
                }

                case bc::Type::Neumann:
                {
                    double manufacturedBoundaryFlux = bc->flux;

                    if (verificationCase)
                    {
                        manufacturedBoundaryFlux = verificationCase->manufacturedBoundaryFlux(face);
                    }

                    flux.addSource( P, -manufacturedBoundaryFlux * face.area, 0.0 );

                    break;
                }

                case bc::Type::Convective:
                {
                    double h = bc->h;
                    double Tinf = bc->Tinf;

                    if (verificationCase)
                    {
                        ConvectiveData robin = verificationCase->manufacturedConvectiveBoundary(face);

                        h = robin.h;
                        Tinf = robin.T_inf;
                    }

                    const double hA = h * face.area;

                    const double H = (hA * D) / (hA + D);

                    flux.addSource( P, H * Tinf, -H );

                    break;
                }

                default:
                    break;
            }
        }
    }
}