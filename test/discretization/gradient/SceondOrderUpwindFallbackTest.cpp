#include "discretization/reconstructors/SecondOrderUpwindReconstruction.hpp"

#include "mesh/QuadMesh2D.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>
#include <memory>

void runSecondOrderUpwindFallbackTest()
{
    // -------------------------------------------------
    // Test mesh
    // -------------------------------------------------
    std::cout << "Testing switch from 2nd to 1st order reconstructor\n for handling boundary stencil.\n\n";

    // Small mesh so we can deliberately select a face
    // whose upwind cell lies against a boundary.
    QuadMesh2D mesh(
        3,      // nx
        3,      // ny
        3.0,    // lx
        3.0     // ly
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

    SecondOrderUpwindReconstruction reconstruction;

    // -------------------------------------------------
    // Find an interior face adjacent to the left boundary
    // -------------------------------------------------

    std::size_t testFace = Face::INVALID;

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor == Face::INVALID) { continue; }

        const std::size_t owner = face.owner;

        // Cell on the left boundary has an x-index of 0.
        if (owner % 3 == 0)
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
    // owner is therefore the upwind cell.
    // Since owner is adjacent to the left boundary,
    // there should be no upstream cell.
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

        assert(stencil.weights.size() == 1);

        assert(stencil.weights[0].first == face.owner);

        assert( std::abs(stencil.weights[0].second - 1.0) < 1e-12 );
    }

    // -------------------------------------------------
    // Negative flux
    //
    // Flow: neighbor -> owner
    //
    // This makes neighbor the upwind cell, so this
    // particular face should NOT use the fallback.
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

        assert(stencil.weights.size() == 2);

        assert(stencil.weights[0].first == face.neighbor);
        assert( std::abs(stencil.weights[0].second - 1.5) < 1e-12 );

        assert(stencil.weights[1].second == -0.5);
    }

    // -------------------------------------------------
    // Result
    // -------------------------------------------------

    std::cout << "SecondOrderUpwindFallback test passed.\n";
}
