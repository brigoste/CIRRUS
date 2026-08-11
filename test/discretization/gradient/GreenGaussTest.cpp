#include "discretization/gradient/GreenGaussGradient.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/QuadMesh2D.hpp"

#include <cmath>
#include <iostream>

bool runGreenGaussTest()
{
    constexpr std::size_t nx = 40;
    constexpr std::size_t ny = 40;

    QuadMesh2D mesh(nx, ny, 1.0, 1.0);

    ScalarField phi(
        "phi",
        mesh,
        FieldLocation::Cell
    );

    VectorField gradient(
        "gradient",
        mesh,
        FieldLocation::Cell
    );

    // Manufactured solution:
    //
    // phi = x + 2y
    //
    // grad(phi) = (1,2)
    
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const Point& center = mesh.cellCenter(c);

        const double x = center[0];
        const double y = center[1];

        phi[c] = x + 2.0*y;
    }

    GreenGaussGradient scheme;

    scheme.compute(
        mesh,
        phi,
        gradient
    );

    double error = 0.0;
    double reference = 0.0;

    for (std::size_t j = 1; j < ny-1; ++j)
    {
        for (std::size_t i = 1; i < nx-1; ++i)
        {
            const std::size_t c = j*nx + i;

            Vector exact(1.0, 2.0);

            Vector diff = gradient[c] - exact;

            error += diff.magnitudeSquared();
            reference += exact.magnitudeSquared();
        }
    }

    const double l2 = std::sqrt(error / reference);
    constexpr double tolerance = 1e-12;
    const bool passed = l2 < tolerance;

    std::cout
        << "  Green-Gauss   "
        << (passed ? "PASS" : "FAIL")
        << "   L2 Error = "
        << l2
        << "\n";

    return passed;
}
