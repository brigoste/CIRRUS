#include "discretization/reconstructors/tvd/TVDReconstruction.hpp"
#include "discretization/reconstructors/tvd/MinmodLimiter.hpp"

#include "mesh/QuadMesh2D.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>

#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) \
            throw std::runtime_error("Test assertion failed: " #condition); \
    } while (false)

void runTVDReconstructionTest()
{
    // -------------------------------------------------
    // Test mesh
    // -------------------------------------------------

    QuadMesh2D mesh(
        4,      // nx
        4,      // ny
        4.0,    // lx
        4.0     // ly
    );

    ScalarField field(
        "Temperature",
        mesh,
        FieldLocation::Cell,
        0.0
    );

    VectorField gradient(
        "Gradient",
        mesh,
        FieldLocation::Cell,
        Vector{0.0, 0.0, 0.0}
    );

    TVDReconstruction reconstruction( std::make_unique<MinmodLimiter>() );

    // -------------------------------------------------
    // Find an interior face with a valid upstream cell
    // for positive flux.
    // -------------------------------------------------

    std::size_t testFace = Face::INVALID;

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor == Face::INVALID) { continue; }

        const std::size_t owner = face.owner;

        // Positive flux -> owner is upwind.
        // Avoid the left boundary so an upstream cell exists.
        if (owner % 4 != 0)
        {
            testFace = f;
            break;
        }
    }

    TEST_ASSERT(testFace != Face::INVALID);

    const Face& face = mesh.face(testFace);

    // -------------------------------------------------
    // Test 1: Constant field
    //
    // TVD should return the upwind value.
    // -------------------------------------------------

    {
        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            field[c] = 5.0;
        }

        const double flux = 1.0;

        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                flux
            );

        TEST_ASSERT(std::abs(reconstructed - 5.0) < 1e-12);
    }

    // -------------------------------------------------
    // Test 2: Linear field
    //
    // phi = x
    //
    // For a uniform linear field:
    //
    // r = 1
    //
    // Minmod:
    //
    // psi(1) = 1
    //
    // Therefore:
    //
    // phi_f = 0.5 phi_U + 0.5 phi_D
    //
    // which should equal the exact face value.
    // -------------------------------------------------

    {
        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            field[c] = mesh.cellCenter(c).x[0];
        }

        const double flux = 1.0;

        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                flux
            );

        const std::size_t upwind = face.owner;
        const std::size_t downwind = face.neighbor;

        const double phiU = field[upwind];
        const double phiD = field[downwind];

        const double expected = 0.5 * phiU + 0.5 * phiD;

        TEST_ASSERT(std::abs(reconstructed - expected) < 1e-12);

        const double exact = face.center.x[0];

        TEST_ASSERT(std::abs(reconstructed - exact) < 1e-12);
    }

    // -------------------------------------------------
    // Test 3: Local extremum
    //
    // Construct:
    //
    // phi_UU = 1
    // phi_U  = 3
    // phi_D  = 2
    //
    // Therefore:
    //
    // r = (3 - 1) / (2 - 3)
    //   = -2
    //
    // Minmod gives:
    //
    // psi = 0
    //
    // Therefore TVD must reduce to first-order
    // upwind:
    //
    // phi_f = phi_U
    // -------------------------------------------------

    {
        // Locate the upstream cell for the selected face.
        const std::size_t upwind = face.owner;

        const auto& upwindCell = mesh.cell(upwind);

        std::size_t upstream = Face::INVALID;

        for (const std::size_t faceIndex : upwindCell.faces)
        {
            if (faceIndex == testFace) { continue; }

            const Face& candidateFace = mesh.face(faceIndex);

            std::size_t candidate = Face::INVALID;

            if (candidateFace.owner == upwind) { candidate = candidateFace.neighbor; }
            else if (candidateFace.neighbor == upwind) { candidate = candidateFace.owner; }

            if (candidate == Face::INVALID) { continue; }

            const Vector dUC = mesh.cellCenter(candidate) - mesh.cellCenter(upwind);

            const Vector direction = mesh.cellCenter(face.neighbor) - mesh.cellCenter(upwind);

            const double projection = LA::dot(dUC, direction);

            if (projection < 0.0)
            {
                upstream = candidate;
                break;
            }
        }

        TEST_ASSERT(upstream != Face::INVALID);

        field[upstream] = 1.0;
        field[upwind]   = 3.0;
        field[face.neighbor] = 2.0;

        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                1.0
            );

        TEST_ASSERT( std::abs(reconstructed - field[upwind]) < 1e-12 );
    }

    // -------------------------------------------------
    // Test 4: Negative flux
    //
    // The neighbor becomes the upwind cell.
    //
    // Use a linear field and verify that the
    // reconstruction remains exact after reversing
    // the flow direction.
    // -------------------------------------------------

    {
        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            field[c] = mesh.cellCenter(c).x[0];
        }

        const double flux = -1.0;

        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                flux
            );

        const double exact = face.center.x[0];

        TEST_ASSERT( std::abs(reconstructed - exact) < 1e-12 );
    }

    // -------------------------------------------------
    // Test 5: Stencil coefficients
    //
    // For r = 1, Minmod gives psi = 1.
    //
    // Therefore:
    //
    // phi_f =
    //     0.5 phi_U
    //   + 0.5 phi_D
    //
    // -------------------------------------------------

    {
        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            field[c] = mesh.cellCenter(c).x[0];
        }

        const ReconstructionStencil stencil =
            reconstruction.stencil(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                1.0
            );

        TEST_ASSERT(stencil.weights.size() == 2);

        TEST_ASSERT( stencil.weights[0].first == face.owner );

        TEST_ASSERT( stencil.weights[1].first == face.neighbor );

        TEST_ASSERT( std::abs(stencil.weights[0].second - 0.5) < 1e-12 );

        TEST_ASSERT( std::abs(stencil.weights[1].second - 0.5) < 1e-12 );
    }

    std::cout << "TVD Reconstruction test passed.\n";
}
