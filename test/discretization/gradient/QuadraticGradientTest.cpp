#include "discretization/gradient/GreenGaussGradient.hpp"
#include "discretization/gradient/LeastSquaresGradient.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/QuadMesh2D.hpp"

#include <cmath>
#include <iostream>

namespace
{

double computeL2Error(
    const MeshBase& mesh,
    const VectorField& gradient
)
{
    double error = 0.0;
    double reference = 0.0;

    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const Point& center = mesh.cellCenter(c);

        const double x = center[0];
        const double y = center[1];

        Vector exact(
            2.0 * x,
            2.0 * y
        );

        Vector diff = gradient[c] - exact;

        error += diff.magnitudeSquared();
        reference += exact.magnitudeSquared();
    }

    return std::sqrt(error / reference);
}

}

bool testGradientScheme(
    const GradientScheme& scheme,
    const std::string& name,
    const MeshBase& mesh,
    const ScalarField& phi,
    VectorField& gradient
)
{
    scheme.compute(
        mesh,
        phi,
        gradient
    );

    const double error = computeL2Error(mesh, gradient);

    double tolerance;

    if (name == "Green-Gauss")
    {
        tolerance = 0.15;
    }
    else if (name == "Least-Squares")
    {
        tolerance = 0.01;
    }
    else
    {
        tolerance = 1e-12;
    }

    const bool passed = error < tolerance;

    std::cout
        << "  "
        << name
        << "   "
        << (passed ? "PASS" : "FAIL")
        << "   L2 Error = "
        << error
        << "\n";

    return passed;
}

bool runQuadraticGradientTest()
{
    constexpr std::size_t nx = 40;
    constexpr std::size_t ny = 40;

    QuadMesh2D mesh(nx, ny, 1.0, 1.0);

    ScalarField phi(
        "phi",
        mesh,
        FieldLocation::Cell
    );

    VectorField ggGradient(
        "ggGradient",
        mesh,
        FieldLocation::Cell
    );

    VectorField lsGradient(
        "lsGradient",
        mesh,
        FieldLocation::Cell
    );


    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const Point& center = mesh.cellCenter(c);

        const double x = center[0];
        const double y = center[1];

        phi[c] = x*x + y*y;
    }


    GreenGaussGradient greenGauss;
    LeastSquaresGradient leastSquares;


    const bool ggPassed = testGradientScheme(
        greenGauss,
        "Green-Gauss",
        mesh,
        phi,
        ggGradient
    );


    const bool lsPassed = testGradientScheme(
        leastSquares,
        "Least-Squares",
        mesh,
        phi,
        lsGradient
    );


    std::cout << "\nQuadratic Gradient Test COMPLETE\n";

    return ggPassed && lsPassed;
}