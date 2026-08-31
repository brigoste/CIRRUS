#include "interpolators/LinearInterpolator.hpp"
#include "interpolators/BilinearInterpolator.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/Mesh1D.hpp"
#include "mesh/QuadMesh2D.hpp"

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

    LinearInterpolator linearInterpolator;

    bool allPassed = true;

    std::size_t testsPassed = 0;
    std::size_t testsFailed = 0;

    std::cout << "\n"
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
        ScalarField phi( "phi",
                         mesh,
                         FieldLocation::Node,
                         5.0 );

        const double x = 0.375;

        Point test{x, 0.0, 0.0};
        const double interpolated = linearInterpolator.interpolate( phi, test );

        const bool passed = nearlyEqual(interpolated, 5.0, tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Constant node field       : "
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
        const double interpolated = linearInterpolator.interpolate( phi, test );

        const bool passed = nearlyEqual( interpolated, exact, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Linear node field         : "
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
        const double interpolated = linearInterpolator.interpolate( phi, test );

        const bool passed = std::abs(interpolated - exact) > 1.0e-6;

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Quadratic node field      : "
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
        const double interpolated = linearInterpolator.interpolate( phi, test );

        const bool passed = nearlyEqual( interpolated, exact, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Face-centered field       : "
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
        const double interpolated = linearInterpolator.interpolate( phi, test );

        const bool passed = nearlyEqual( interpolated, exact, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Cell-centered field       : "
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
            const double interpolated = linearInterpolator.interpolate( phi, test );

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

        std::cout << "Exact field locations     : "
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
            linearInterpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        try
        {
            Point test{1.1, 0.0, 0.0};
            linearInterpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Outside interpolation     : "
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
            linearInterpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        try
        {
            Point test{1.0, 0.0, 0.0};
            linearInterpolator.interpolate( phi, test );

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Cell boundary rejection   : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Summary
     * ------------------------------------------------------------
     */
    std::cout << "\n"
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

bool runBilinearInterpolatorTest()
{
    constexpr double lengthX = 1.0;
    constexpr double lengthY = 1.0;

    constexpr std::size_t nx = 10;
    constexpr std::size_t ny = 10;

    constexpr double tolerance = 1.0e-12;

    QuadMesh2D mesh(nx, ny, lengthX, lengthY);

    BilinearInterpolator bilinearInterpolator;

    bool allPassed = true;

    std::size_t testsPassed = 0;
    std::size_t testsFailed = 0;

    std::cout << "\n"
              << "============================================================\n"
              << "Bilinear Interpolator Test\n"
              << "============================================================\n"
              << "\n"
              << "Mesh:\n"
              << "  Cells X : " << nx << "\n"
              << "  Cells Y : " << ny << "\n"
              << "  Length X: " << lengthX << "\n"
              << "  Length Y: " << lengthY << "\n"
              << "\n"
              << "Tolerance:\n"
              << "  " << std::scientific << tolerance << "\n"
              << "\n";

    /*
     * ------------------------------------------------------------
     * Linear node-centered field
     * ------------------------------------------------------------
     *
     *     phi(x,y) = 2x + 3y + 1
     *
     * Bilinear interpolation should reproduce a linear field
     * exactly.
     */
    {
        ScalarField phi( "phi",
                         mesh,
                         FieldLocation::Node );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const Point& p = mesh.node(i);

            phi[i] = 2.0 * p[0] + 3.0 * p[1] + 1.0;
        }

        Point test{0.2676, 0.5, 0.0};

        const double exact = 2.0 * test[0] +
                             3.0 * test[1] +
                             1.0;

        const double interpolated = bilinearInterpolator.interpolate(phi, test);

        const bool passed = nearlyEqual(interpolated, exact, tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Linear node scalar field : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Bilinear node-centered field
     * ------------------------------------------------------------
     *
     *     phi(x,y) = 1 + 2x + 3y + 4xy
     *
     * Bilinear interpolation should reproduce this exactly.
     */
    {
        ScalarField phi( "phi",
                         mesh,
                         FieldLocation::Node );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const Point& p = mesh.node(i);

            const double x = p[0];
            const double y = p[1];

            phi[i] = 1.0 +
                     2.0 * x +
                     3.0 * y +
                     4.0 * x * y;
        }

        Point test{0.2676, 0.5, 0.0};

        const double x = test[0];
        const double y = test[1];

        const double exact = 1.0 +
                             2.0 * x +
                             3.0 * y +
                             4.0 * x * y;

        const double interpolated = bilinearInterpolator.interpolate(phi, test);

        const bool passed = nearlyEqual(interpolated, exact, tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Bilinear node scalar     : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Bilinear node-centered vector field
     * ------------------------------------------------------------
     *
     * Each component is bilinear in x and y.
     */
    {
        VectorField field( "vector",
                           mesh,
                           FieldLocation::Node );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const Point& p = mesh.node(i);

            const double x = p[0];
            const double y = p[1];

            field[i] = Vector( 1.0 + x + 2.0 * y,
                               2.0 + 3.0 * x + y,
                               3.0 + 2.0 * x * y );
        }

        Point test{0.2676, 0.5, 0.0};

        const double x = test[0];
        const double y = test[1];

        const Vector exact( 1.0 + x + 2.0 * y,
                            2.0 + 3.0 * x + y,
                            3.0 + 2.0 * x * y
        );

        const Vector interpolated = bilinearInterpolator.interpolate(field, test);

        const bool passed = nearlyEqual(interpolated[0], exact[0], tolerance) &&
                            nearlyEqual(interpolated[1], exact[1], tolerance) &&
                            nearlyEqual(interpolated[2], exact[2], tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Bilinear node vector     : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    
    /*
    * ------------------------------------------------------------
    * 2D Face-Centered Scalar Field
    * ------------------------------------------------------------
    *
    *     phi(x,y) = 2x + 3y + 1
    *
    * The face-centered values are sampled at the actual face
    * centers. Bilinear interpolation should reproduce a linear
    * field exactly.
    */
    {
        QuadMesh2D mesh2D( 10,
                           10,
                           1.0,
                           1.0 );

        ScalarField field( "phi",
                           mesh2D,
                           FieldLocation::Face );

        for (std::size_t i = 0; i < mesh2D.nfaces(); ++i)
        {
            const double x = mesh2D.face(i).center[0];
            const double y = mesh2D.face(i).center[1];

            field[i] = 2.0 * x + 3.0 * y + 1.0;
        }

        const Point test{ 0.375, 0.625, 0.0 };

        const double expected = 2.0 * test[0]
                              + 3.0 * test[1]
                              + 1.0;

        const double interpolated = bilinearInterpolator.interpolate(field, test);

        const bool passed = nearlyEqual(interpolated, expected, tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Face-centered scalar     : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    return allPassed;
}
