#include "interpolators/LinearInterpolator.hpp"

#include "fields/ScalarField.hpp"

#include "mesh/Mesh1D.hpp"
#include "mesh/primitives/Point.hpp"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>

namespace
{

    bool nearlyEqual(
        double a,
        double b,
        double tolerance = 1.0e-12
    )
    {
        return std::abs(a - b) <= tolerance;
    }

} // namespace

bool runInterpolatorTest()
{
    constexpr double length = 1.0;
    constexpr std::size_t nCells = 10;
    constexpr double tolerance = 1.0e-12;

    Mesh1D mesh(nCells, length);

    LinearInterpolator interpolator;

    bool allPassed = true;

    std::size_t testsPassed = 0;
    std::size_t testsFailed = 0;

    std::cout
        << "\n"
        << "============================================================\n"
        << "Linear Interpolator Test\n"
        << "============================================================\n"
        << "\n"
        << "Mesh:\n"
        << "  Cells : " << mesh.ncells() << "\n"
        << "  Length: " << length << "\n"
        << "\n"
        << "Tolerance:\n"
        << "  " << std::scientific << tolerance << "\n"
        << "\n";

    /*
     * ------------------------------------------------------------
     * Constant node field
     * ------------------------------------------------------------
     *
     *     phi(x) = 5
     *
     * Linear interpolation should preserve a constant field
     * exactly.
     */
    {
        ScalarField phi(
            "phi",
            mesh,
            FieldLocation::Node,
            5.0
        );

        const double x = 0.375;

        Point test{x, 0.0, 0.0};
        const double interpolated = interpolator.interpolate( phi, test );

        const bool passed = nearlyEqual(interpolated, 5.0, tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Constant node field       : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Linear node field
     * ------------------------------------------------------------
     *
     *     phi(x) = 2x + 1
     *
     * Linear interpolation should reproduce a linear field
     * exactly.
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Node );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            phi[i] = 2.0 * x + 1.0;
        }

        const double x = 0.375;

        const double exact = 2.0 * x + 1.0;

        Point test{x, 0.0, 0.0};
        const double interpolated = interpolator.interpolate( phi, test );

        const bool passed = nearlyEqual( interpolated, exact, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Linear node field         : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Quadratic node field
     * ------------------------------------------------------------
     *
     *     phi(x) = x^2
     *
     * Linear interpolation is not exact for a quadratic field.
     *
     * This verifies that the implementation is actually performing
     * linear interpolation rather than evaluating the analytical
     * function directly.
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Node );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            phi[i] = x * x;
        }

        const double x = 0.375;

        const double exact = x * x;

        Point test{x, 0.0, 0.0};
        const double interpolated = interpolator.interpolate( phi, test );

        const bool passed = std::abs(interpolated - exact) > 1.0e-6;

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Quadratic node field      : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Face-centered field
     * ------------------------------------------------------------
     *
     *     phi(x) = 2x + 1
     *
     * Verify interpolation using face-centered field values.
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Face );

        for (std::size_t i = 0; i < mesh.nfaces(); ++i)
        {
            const double x = mesh.face(i).center[0];
            phi[i] = 2.0 * x + 1.0;
        }

        const double x = 0.35;

        const double exact = 2.0 * x + 1.0;

        Point test{x, 0.0, 0.0};
        const double interpolated = interpolator.interpolate( phi, test );

        const bool passed = nearlyEqual( interpolated, exact, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Face-centered field       : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Cell-centered field
     * ------------------------------------------------------------
     *
     *     phi(x) = 2x + 1
     *
     * Verify interpolation between cell centers.
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Cell );

        for (std::size_t i = 0; i < mesh.ncells(); ++i)
        {
            const double x = mesh.cellCenter(i)[0];

            phi[i] = 2.0 * x + 1.0;
        }

        const double x = 0.25;

        const double exact = 2.0 * x + 1.0;

        Point test{x, 0.0, 0.0};
        const double interpolated = interpolator.interpolate( phi, test );

        const bool passed = nearlyEqual( interpolated, exact, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Cell-centered field       : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Exact field locations
     * ------------------------------------------------------------
     *
     * Interpolating at an existing field location should return
     * the stored field value exactly.
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Node );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            phi[i] = 3.0 * x - 2.0;
        }

        bool passed = true;

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            Point test{x, 0.0, 0.0};
            const double interpolated = interpolator.interpolate( phi, test );

            if (!nearlyEqual( interpolated, phi[i], tolerance))
            {
                passed = false;
                break;
            }
        }

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Exact field locations     : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Outside interpolation range
     * ------------------------------------------------------------
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Node, 1.0 );

        bool passed = true;

        try
        {
            Point test{-0.1, 0.0, 0.0};
            interpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        try
        {
            Point test{1.1, 0.0, 0.0};
            interpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        if (passed)
        {
            ++testsPassed;
        }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Outside interpolation     : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Cell-centered boundary rejection
     * ------------------------------------------------------------
     *
     * Cell centers do not exist at the physical boundaries.
     * Therefore the initial interpolator should reject these
     * requests rather than silently extrapolating.
     */
    {
        ScalarField phi( "phi", mesh, FieldLocation::Cell, 1.0 );

        bool passed = true;

        try
        {
            Point test{0.0, 0.0, 0.0};
            interpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        try
        {
            Point test{1.0, 0.0, 0.0};
            interpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        if (passed)
        {
            ++testsPassed;
        }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout
            << "Cell boundary rejection   : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Summary
     * ------------------------------------------------------------
     */
    std::cout
        << "\n"
        << "------------------------------------------------------------\n"
        << "Interpolator Test Results\n"
        << "------------------------------------------------------------\n"
        << "Tests passed              : " << testsPassed << "\n"
        << "Tests failed              : " << testsFailed << "\n"
        << "\n"
        << "============================================================\n"
        << "Linear Interpolator Test "
        << (allPassed ? "PASS" : "FAIL")
        << "\n"
        << "============================================================\n";

    return allPassed;
}