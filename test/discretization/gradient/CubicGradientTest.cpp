#include "discretization/gradient/GreenGaussGradient.hpp"
#include "discretization/gradient/LeastSquaresGradient.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/QuadMesh2D.hpp"

#include <cmath>
#include "discretization/gradient/GreenGaussGradient.hpp"
#include "discretization/gradient/LeastSquaresGradient.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/QuadMesh2D.hpp"

#include <cmath>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct RefinementResult
{
    int nx;
    double h;
    double gg_error;
    double ls_error;
};

namespace
{
    double computeInteriorL2Error( const MeshBase& mesh, const VectorField& gradient, std::size_t nx, std::size_t ny)
    {
        double error = 0.0;
        double reference = 0.0;

        for (std::size_t j = 1; j < ny - 1; ++j)
        {
            for (std::size_t i = 1; i < nx - 1; ++i)
            {
                const std::size_t c = j * nx + i;

                const Point& center = mesh.cellCenter(c);

                const double x = center[0];
                const double y = center[1];

                Vector exact( 3.0 * x * x,
                              3.0 * y * y );

                Vector diff = gradient[c] - exact;

                error += diff.magnitudeSquared();
                reference += exact.magnitudeSquared();
            }
        }

        return std::sqrt(error / reference);
    }

    double computeOrder( double coarseError, double fineError)
    {
        return std::log(coarseError / fineError) / std::log(2.0);
    }

} // namespace

bool runCubicGradientTest()
{
    constexpr double lx = 1.0;
    constexpr double ly = 1.0;

    constexpr double ggTolerance = 0.15;
    constexpr double lsTolerance = 0.01;

    const std::vector<std::size_t> refinementLevels = { 20,
                                                        40,
                                                        80,
                                                        160 };

    GreenGaussGradient greenGauss;
    LeastSquaresGradient leastSquares;

    std::vector<RefinementResult> results;
    results.reserve(refinementLevels.size());

    bool allPassed = true;

    std::cout << "\n"
              << "============================================================\n"
              << "Quadratic Gradient Verification\n"
              << "============================================================\n"
              << "\n"
              << "Manufactured field:\n"
              << "  phi(x,y) = x^3 + y^3\n"
              << "  grad(phi) = (3x^2, 3y^2)\n"
              << "\n"
              << "Error metric:\n"
              << "  Normalized L2 error over interior cells only\n"
              << "\n"
              << "Acceptance tolerances:\n"
              << "  Green-Gauss   < " << std::scientific << ggTolerance << "\n"
              << "  Least-Squares < " << std::scientific << lsTolerance << "\n"
              << "\n"
              << "------------------------------------------------------------\n"
              << "Mesh Refinement\n"
              << "------------------------------------------------------------\n";

    std::cout << std::left
              << std::setw(12) << "Mesh"
              << std::setw(16) << "h"
              << std::setw(22) << "Green-Gauss"
              << std::setw(22) << "Least-Squares"
              << "\n";

    std::cout << "------------------------------------------------------------\n";

    for (const std::size_t nx : refinementLevels)
    {
        QuadMesh2D mesh(nx, nx, lx, ly);

        ScalarField phi( "phi",
                         mesh,
                         FieldLocation::Cell );

        VectorField ggGradient( "ggGradient",
                                mesh,
                                FieldLocation::Cell );

        VectorField lsGradient( "lsGradient",
                                mesh,
                                FieldLocation::Cell );

        for (std::size_t c = 0; c < mesh.ncells(); ++c)
        {
            const Point& center = mesh.cellCenter(c);

            const double x = center[0];
            const double y = center[1];

            phi[c] = (x * x * x) + (y * y * y);
        }

        greenGauss.compute( mesh,
                            phi,
                            ggGradient );

        leastSquares.compute( mesh,
                              phi,
                              lsGradient );

        const double ggError = computeInteriorL2Error( mesh,
                                                       ggGradient,
                                                       nx,
                                                       nx );

        const double lsError = computeInteriorL2Error( mesh,
                                                       lsGradient,
                                                       nx,
                                                       nx );

        const bool ggPassed = ggError < ggTolerance;
        const bool lsPassed = lsError < lsTolerance;

        allPassed = allPassed && ggPassed && lsPassed;

        const double h = lx / static_cast<double>(nx);

        results.push_back( { static_cast<int>(nx),
                             h,
                             ggError,
                             lsError } );

        std::cout << std::left
                  << std::setw(12)
                  << (std::to_string(nx) + "x" + std::to_string(nx))
                  << std::scientific
                  << std::setprecision(6)
                  << std::setw(16)
                  << h
                  << std::setw(22)
                  << (std::to_string(ggError) + (ggPassed ? " PASS" : " FAIL"))
                  << std::setw(22)
                  << (std::to_string(lsError) + (lsPassed ? " PASS" : " FAIL"))
                  << "\n";
    }

    std::cout << "\n"
              << "------------------------------------------------------------\n"
              << "Observed Order\n"
              << "------------------------------------------------------------\n";

    std::cout << std::left
              << std::setw(18) << "Refinement"
              << std::setw(20) << "Green-Gauss"
              << std::setw(20) << "Least-Squares"
              << "\n";

    std::cout << "------------------------------------------------------------\n";

    for (std::size_t i = 1; i < results.size(); ++i)
    {
        const double ggOrder = computeOrder( results[i - 1].gg_error, results[i].gg_error );

        const double lsOrder = computeOrder( results[i - 1].ls_error, results[i].ls_error );

        std::cout << std::left
                  << std::setw(18)
                  << ( std::to_string(results[i - 1].nx) + " -> " + std::to_string(results[i].nx) )
                  << std::fixed
                  << std::setprecision(4)
                  << std::setw(20)
                  << ggOrder
                  << std::setw(20)
                  << lsOrder
                  << "\n";
    }

    std::cout << "\n"
              << "============================================================\n"
              << "Quadratic Gradient Verification "
              << (allPassed ? "PASS" : "FAIL")
              << "\n"
              << "============================================================\n";

    return allPassed;
}
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

        Vector exact( 2.0 * x, 2.0 * y );

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
    scheme.compute( mesh,
                    phi,
                    gradient );

    const double error = computeL2Error(mesh, gradient);

    double tolerance;

    if (name == "Green-Gauss") { tolerance = 0.15; }
    else if (name == "Least-Squares") { tolerance = 0.01; }
    else { tolerance = 1e-12; }

    const bool passed = error < tolerance;

    std::cout << "  "
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

    ScalarField phi( "phi",
                     mesh,
                     FieldLocation::Cell );

    VectorField ggGradient( "ggGradient",
                            mesh,
                            FieldLocation::Cell );

    VectorField lsGradient( "lsGradient",
                            mesh,
                            FieldLocation::Cell );


    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const Point& center = mesh.cellCenter(c);

        const double x = center[0];
        const double y = center[1];

        phi[c] = x*x + y*y;
    }


    GreenGaussGradient greenGauss;
    LeastSquaresGradient leastSquares;


    const bool ggPassed = testGradientScheme( greenGauss,
                                              "Green-Gauss",
                                              mesh,
                                              phi,
                                              ggGradient );


    const bool lsPassed = testGradientScheme( leastSquares,
                                              "Least-Squares",
                                              mesh,
                                              phi,
                                              lsGradient );


    std::cout << "\nQuadratic Gradient Test COMPLETE\n";

    return ggPassed && lsPassed;
}
