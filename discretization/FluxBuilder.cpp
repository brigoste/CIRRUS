#include "discretization/FluxAccumulator.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/MeshGeometry.hpp"
#include "physics/PhysicsModel.hpp"
#include "physics/HeatPhysicsModel.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "discretization/FluxBuilder.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include <limits>
#include <cmath>
#include <iostream>

// Look at the chatGPT thread. We may need to add a geometry helper class.

void FluxBuilder::buildFlux(
    const MeshBase& mesh,
    const PhysicsModel& model,
    const BoundaryPatchSystem& boundary,
    FluxAccumulator& flux,
    const VerificationCase* verificationCase = nullptr)
{
    if (flux.size() != mesh.ncells())
        throw std::runtime_error("FluxAccumulator size mismatch");

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
            const double D = model.diffusionCoeff(face);

            flux.addDiffusion(P, N, D);
        }
    }

    // =====================================================
    // 2. BOUNDARY FACES (GROUP-BASED)
    // =====================================================
    for (std::size_t g = 0; g < mesh.nBoundaryGroups(); ++g)
    {
        const auto* bc = boundary.getGroup(g);
        if (!bc)
            continue;

        const auto& faces = mesh.boundaryFaces(g);

        for (std::size_t f : faces)
        {
            const Face& face = mesh.face(f);
            const std::size_t P = face.owner;

            const double D = model.diffusionCoeff(face);

            // flux.addDiffusion(P, P, D);

            switch (bc->type)
            {
                case bc::Type::Dirichlet:
                {
                    double value = bc->value;

                    if (verificationCase)
                    {
                        const auto& xc = mesh.cellCenter(P); // or face centroid (better)

                        double x = xc.x[0];
                        double y = xc.x[1];

                        value = verificationCase->exact(x, y);
                    }

                    flux.addBoundaryDiffusion(P, D, value);
                    break;
                }

                case bc::Type::Neumann:
                    flux.addSource(P, bc->flux * face.area, 0.0);
                    break;

                case bc::Type::Convective:
                {
                    const double hA = bc->h * face.area;
                    flux.addSource(P, hA * bc->Tinf, -hA);
                    break;
                }

                default:
                    break;
            }
        }
    }

    // =====================================================
    // 3. CELL SOURCES
    // =====================================================
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        if (verificationCase)
        {
            const auto& xc = mesh.cellCenter(c);

            const double x = xc.x[0];
            const double y = xc.x[1];

            const double S = verificationCase->source(x, y);

            flux.addSource(c, S * mesh.cellVolume(c), 0.0);
            // std::cout
            //     << "S(raw) = "
            //     << verificationCase->source(x,y)
            //     << "\n"
            //     << "S*V = "
            //     << S * mesh.cellVolume(c)
            //     << "\n";
        }
        else
        {
            model.addCellSources(mesh, c, flux);
        }
    }

    std::cout << "k = " << model.diffusionScalar() << "\n";


#ifdef DEBUG
    for (std::size_t i = 0; i < flux.size(); ++i)
    {
        const auto& c = flux[i];

        if (!std::isfinite(c.Su) || !std::isfinite(c.Sp))
            throw std::runtime_error("Non-finite flux source term");

        if (std::abs(c.Sp) > 1e12)
            std::cerr << "[WARN] stiff source at cell " << i << "\n";
    }

    if (N != Face::INVALID)
    {
        double dotVal = LA::dot(face.dPN, face.normal);
        if (dotVal <= 0.0)
            throw std::runtime_error("Invalid face orientation / geometry");
    }
    
#endif
}
