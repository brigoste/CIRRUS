#include "discretization/reconstructors/MusclReconstruction.hpp"

#include "mesh/QuadMesh2D.hpp"
#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"
#include "mesh/primitives/Face.hpp"

#include <cassert>
#include <cmath>
#include <cstddef>
#include <iostream>

void runMusclReconstructionTest()
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

    MusclReconstruction reconstruction;

    // -------------------------------------------------
    // Find an interior face with a valid upstream cell
    // for positive flow.
    // -------------------------------------------------

    std::size_t testFace = Face::INVALID;

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor == Face::INVALID) { continue; }

        const std::size_t owner = face.owner;

        // For positive flux, owner is upwind.
        //
        // Avoid the left boundary so the owner has an
        // upstream cell.
        if (owner % 4 != 0)
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
    // Flow direction:
    //
    //     upstream -> owner -> neighbor
    //                    |
    //                    F
    //
    // -------------------------------------------------

    const double positiveFlux = 1.0;

    const ReconstructionStencil positiveStencil =
        reconstruction.stencil(
            mesh,
            face.owner,
            testFace,
            field,
            gradient,
            positiveFlux
        );

    // -------------------------------------------------
    // Verify stencil contains three cells
    // -------------------------------------------------

    assert(positiveStencil.weights.size() == 3);

    // -------------------------------------------------
    // Verify coefficients
    // -------------------------------------------------

    // assert( std::abs(positiveStencil.weights[0].second + 0.25) < 1e-12 );
    // assert( std::abs(positiveStencil.weights[1].second - 0.75) < 1e-12 );
    // assert( std::abs(positiveStencil.weights[2].second - 0.50) < 1e-12 );
    assert(std::abs(positiveStencil.weights[0].second + 0.125) < 1e-12);
    assert(std::abs(positiveStencil.weights[1].second - 0.750) < 1e-12);
    assert(std::abs(positiveStencil.weights[2].second - 0.375) < 1e-12);
    

    // Coefficients must form a consistent interpolation.
    double coefficientSum = 0.0;

    for (const auto& [cell, weight] : positiveStencil.weights)
    {
        (void)cell;
        coefficientSum += weight;
    }

    assert( std::abs(coefficientSum - 1.0) < 1e-12 );

    // -------------------------------------------------
    // Verify upwind/downwind cell ordering
    // -------------------------------------------------

    const std::size_t positiveUpwind = face.owner;
    const std::size_t positiveDownwind = face.neighbor;

    assert( positiveStencil.weights[1].first == positiveUpwind );
    assert( positiveStencil.weights[2].first == positiveDownwind );

    // The first stencil entry should be a cell other
    // than the upwind/downwind cells.
    assert( positiveStencil.weights[0].first != positiveUpwind );
    assert( positiveStencil.weights[0].first != positiveDownwind );

    // -------------------------------------------------
    // Negative flux
    //
    // Flow direction reverses:
    //
    //     upstream -> neighbor -> owner
    // -------------------------------------------------

    const double negativeFlux = -1.0;

    const ReconstructionStencil negativeStencil =
        reconstruction.stencil(
            mesh,
            face.owner,
            testFace,
            field,
            gradient,
            negativeFlux
        );

    assert(negativeStencil.weights.size() == 3);

    // Same coefficient structure.
    // assert( std::abs(negativeStencil.weights[0].second + 0.25) < 1e-12 );
    // assert( std::abs(negativeStencil.weights[1].second - 0.75) < 1e-12 );
    // assert( std::abs(negativeStencil.weights[2].second - 0.50) < 1e-12 );
    assert(std::abs(negativeStencil.weights[0].second + 0.125) < 1e-12);
    assert(std::abs(negativeStencil.weights[1].second - 0.750) < 1e-12);
    assert(std::abs(negativeStencil.weights[2].second - 0.375) < 1e-12);

    // Coefficients still sum to one.
    coefficientSum = 0.0;

    for (const auto& [cell, weight] : negativeStencil.weights)
    {
        (void)cell;
        coefficientSum += weight;
    }

    assert( std::abs(coefficientSum - 1.0) < 1e-12 );

    // Negative flow means the neighbor is now upwind.
    const std::size_t negativeUpwind = face.neighbor;
    const std::size_t negativeDownwind = face.owner;

    assert( negativeStencil.weights[1].first == negativeUpwind );
    assert( negativeStencil.weights[2].first == negativeDownwind );
    assert( negativeStencil.weights[0].first != negativeUpwind );
    assert( negativeStencil.weights[0].first != negativeDownwind );

    // -------------------------------------------------
    // Linear-field exactness
    //
    // phi(x,y) = x
    //
    // For a linear field, the reconstructed face value
    // should equal the analytical value at the face.
    // -------------------------------------------------

    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        field[c] = mesh.cellCenter(c).x[0];
    }

    // -------------------------------------------------
    // Positive-flow reconstruction
    // -------------------------------------------------

    {
        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                positiveFlux
            );

        const double exact = face.center.x[0];

        assert( std::abs(reconstructed - exact) < 1e-12 );
    }

    // -------------------------------------------------
    // Negative-flow reconstruction
    // -------------------------------------------------

    {
        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                negativeFlux
            );

        const double exact = face.center.x[0];

        assert(
            std::abs(reconstructed - exact)
            < 1e-12
        );
    }

    // -------------------------------------------------
    // Verify reconstruct() agrees with stencil()
    //
    // This is an implementation-consistency test.
    // It is intentionally separate from the analytical
    // accuracy test above.
    // -------------------------------------------------

    {
        const ReconstructionStencil stencil =
            reconstruction.stencil(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                positiveFlux
            );

        double expected = 0.0;

        for (const auto& [cell, weight] : stencil.weights)
        {
            expected += weight * field[cell];
        }

        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                face.owner,
                testFace,
                field,
                gradient,
                positiveFlux
            );

        assert(
            std::abs(reconstructed - expected)
            < 1e-12
        );
    }

    // -------------------------------------------------
    // Result
    // -------------------------------------------------

    std::cout
        << "MUSCL Reconstruction test passed.\n";
}
