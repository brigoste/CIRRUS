#include "discretization/reconstructors/higher_order/QuickReconstruction.hpp"

#include "mesh/QuadMesh2D.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

void runQuickReconstructionTest()
{
    // -------------------------------------------------
    // Test mesh
    // -------------------------------------------------

    std::cout << "Testing QUICK reconstruction stencil.\n\n";

    QuadMesh2D mesh(
        5,      // nx
        5,      // ny
        5.0,    // lx
        5.0     // ly
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

    QuickReconstruction reconstruction;

    // -------------------------------------------------
    // Find an interior face away from the boundary
    // -------------------------------------------------

    std::size_t testFace = Face::INVALID;

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor == Face::INVALID) { continue; }

        const std::size_t owner = face.owner;

        // Avoid cells on the outermost rows/columns.
        const std::size_t i = owner % 5;
        const std::size_t j = owner / 5;

        if (i > 0 && i < 4 && j > 0 && j < 4)
        {
            testFace = f;
            break;
        }
    }

    assert(testFace != Face::INVALID);

    const Face& face = mesh.face(testFace);

    // -------------------------------------------------
    // Positive flux
    //
    // Flow: owner -> neighbor
    //
    // QUICK stencil:
    //
    //     -1/8 * upstream
    //     +6/8 * upwind
    //     +3/8 * downwind
    // -------------------------------------------------

    {
        const double flux = 1.0;

        const ReconstructionStencil stencil =
            reconstruction.stencil(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                flux
            );

        assert(stencil.weights.size() == 3);

        assert( std::abs(stencil.weights[0].second + 1.0 / 8.0) < 1e-12 );

        assert( std::abs(stencil.weights[1].second - 6.0 / 8.0) < 1e-12 );

        assert( std::abs(stencil.weights[2].second - 3.0 / 8.0) < 1e-12 );

        // QUICK coefficients must preserve a constant field.
        double weightSum = 0.0;

        for (const auto& [cell, weight] : stencil.weights)
        {
            (void)cell;
            weightSum += weight;
        }

        assert(std::abs(weightSum - 1.0) < 1e-12);
    }

    // -------------------------------------------------
    // Negative flux
    //
    // Flow: neighbor -> owner
    //
    // The stencil orientation should reverse.
    // -------------------------------------------------

    {
        const double flux = -1.0;

        const ReconstructionStencil stencil =
            reconstruction.stencil(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                flux
            );

        assert(stencil.weights.size() == 3);

        assert( std::abs(stencil.weights[0].second + 1.0 / 8.0) < 1e-12 );

        assert( std::abs(stencil.weights[1].second - 6.0 / 8.0) < 1e-12 );

        assert( std::abs(stencil.weights[2].second - 3.0 / 8.0) < 1e-12 );

        double weightSum = 0.0;

        for (const auto& [cell, weight] : stencil.weights)
        {
            (void)cell;
            weightSum += weight;
        }

        assert(std::abs(weightSum - 1.0) < 1e-12);

        // The upwind cell should switch when the flux reverses.
        assert( stencil.weights[1].first == face.owner || stencil.weights[1].first == face.neighbor );
    }

    // -------------------------------------------------
    // Result
    // -------------------------------------------------

    std::cout << "QuickReconstruction test passed.\n";
}
