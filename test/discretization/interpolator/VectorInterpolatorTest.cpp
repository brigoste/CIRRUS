#include "interpolators/LinearInterpolator.hpp"

#include "fields/VectorField.hpp"

#include "mesh/Mesh1D.hpp"
#include "mesh/QuadMesh2D.hpp"
#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Vector.hpp"

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

bool vectorsNearlyEqual(
    const Vector& a,
    const Vector& b,
    double tolerance = 1.0e-12
)
{
    for (std::size_t i = 0; i < 3; ++i)
    {
        if (!nearlyEqual(a[i], b[i], tolerance))
        {
            return false;
        }
    }

    return true;
}

} // namespace

bool runVectorInterpolatorTest()
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
        << "Vector Linear Interpolator Test\n"
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
     * Constant node vector field
     * ------------------------------------------------------------
     *
     *     U(x) = [ 2, -3, 5 ]
     *
     * Linear interpolation should preserve a constant field
     * exactly.
     */
    {
        Vector value;

        value[0] = 2.0;
        value[1] = -3.0;
        value[2] = 5.0;

        VectorField field(
            "U",
            mesh,
            FieldLocation::Node,
            value
        );

        const double x = 0.375;

        Point test{x, 0.0, 0.0};

        const Vector interpolated =
            interpolator.interpolate(field, test);

        const bool passed =
            vectorsNearlyEqual(interpolated, value, tolerance);

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
     * Linear node vector field
     * ------------------------------------------------------------
     *
     *     U(x) =
     *
     *     [ 2x + 1,
     *      -3x + 4,
     *       0.5x - 2 ]
     *
     * Linear interpolation should reproduce every component
     * exactly.
     */
    {
        VectorField field(
            "U",
            mesh,
            FieldLocation::Node
        );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            field[i][0] = 2.0 * x + 1.0;
            field[i][1] = -3.0 * x + 4.0;
            field[i][2] = 0.5 * x - 2.0;
        }

        const double x = 0.375;

        Vector expected;

        expected[0] = 2.0 * x + 1.0;
        expected[1] = -3.0 * x + 4.0;
        expected[2] = 0.5 * x - 2.0;

        Point test{x, 0.0, 0.0};

        const Vector interpolated = interpolator.interpolate(field, test);

        const bool passed = vectorsNearlyEqual(interpolated, expected, tolerance);

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
     * Quadratic node vector field
     * ------------------------------------------------------------
     *
     *     U(x) =
     *
     *     [ x^2,
     *       2x^2,
     *      -x^2 ]
     *
     * Linear interpolation should NOT reproduce the analytical
     * quadratic field exactly.
     */
    {
        VectorField field(
            "U",
            mesh,
            FieldLocation::Node
        );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            field[i][0] = x * x;
            field[i][1] = 2.0 * x * x;
            field[i][2] = -x * x;
        }

        const double x = 0.375;

        Vector exact;

        exact[0] = x * x;
        exact[1] = 2.0 * x * x;
        exact[2] = -x * x;

        Point test{x, 0.0, 0.0};

        const Vector interpolated = interpolator.interpolate(field, test);

        const bool passed = !vectorsNearlyEqual(interpolated, exact, 1.0e-6);

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
     * Face-centered vector field
     * ------------------------------------------------------------
     */
    {
        VectorField field(
            "U",
            mesh,
            FieldLocation::Face
        );

        for (std::size_t i = 0; i < mesh.nfaces(); ++i)
        {
            const double x = mesh.face(i).center[0];

            field[i][0] = 2.0 * x + 1.0;
            field[i][1] = -3.0 * x + 4.0;
            field[i][2] = 0.5 * x - 2.0;
        }

        const double x = 0.35;

        Vector expected;

        expected[0] = 2.0 * x + 1.0;
        expected[1] = -3.0 * x + 4.0;
        expected[2] = 0.5 * x - 2.0;

        Point test{x, 0.0, 0.0};

        const Vector interpolated =
            interpolator.interpolate(field, test);

        const bool passed =
            vectorsNearlyEqual(interpolated, expected, tolerance);

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
     * Cell-centered vector field
     * ------------------------------------------------------------
     */
    {
        VectorField field(
            "U",
            mesh,
            FieldLocation::Cell
        );

        for (std::size_t i = 0; i < mesh.ncells(); ++i)
        {
            const double x = mesh.cellCenter(i)[0];

            field[i][0] = 2.0 * x + 1.0;
            field[i][1] = -3.0 * x + 4.0;
            field[i][2] = 0.5 * x - 2.0;
        }

        const double x = 0.25;

        Vector expected;

        expected[0] = 2.0 * x + 1.0;
        expected[1] = -3.0 * x + 4.0;
        expected[2] = 0.5 * x - 2.0;

        Point test{x, 0.0, 0.0};

        const Vector interpolated = interpolator.interpolate(field, test);

        const bool passed = vectorsNearlyEqual(interpolated, expected, tolerance);

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
     */
    {
        VectorField field(
            "U",
            mesh,
            FieldLocation::Node
        );

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            field[i][0] = 3.0 * x - 2.0;
            field[i][1] = 4.0 * x + 1.0;
            field[i][2] = -2.0 * x + 5.0;
        }

        bool passed = true;

        for (std::size_t i = 0; i < mesh.nnodes(); ++i)
        {
            const double x = mesh.node(i)[0];

            Point test{x, 0.0, 0.0};

            const Vector interpolated = interpolator.interpolate(field, test);

            if (!vectorsNearlyEqual(interpolated, field[i], tolerance))
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
        Vector value;

        value[0] = 1.0;
        value[1] = 2.0;
        value[2] = 3.0;

        VectorField field(
            "U",
            mesh,
            FieldLocation::Node,
            value
        );

        bool passed = true;

        try
        {
            Point test{-0.1, 0.0, 0.0};
            interpolator.interpolate(field, test);

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        try
        {
            Point test{1.1, 0.0, 0.0};
            interpolator.interpolate(field, test);

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

        std::cout
            << "Outside interpolation     : "
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Cell-centered boundary rejection
     * ------------------------------------------------------------
     */
    {
        Vector value;

        value[0] = 1.0;
        value[1] = 2.0;
        value[2] = 3.0;

        VectorField field(
            "U",
            mesh,
            FieldLocation::Cell,
            value
        );

        bool passed = true;

        try
        {
            Point test{0.0, 0.0, 0.0};
            interpolator.interpolate(field, test);

            passed = false;
        }
        catch (const std::out_of_range&)
        {
            // Expected.
        }

        try
        {
            Point test{1.0, 0.0, 0.0};
            interpolator.interpolate(field, test);

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
        << "Vector Interpolator Test Results\n"
        << "------------------------------------------------------------\n"
        << "Tests passed              : " << testsPassed << "\n"
        << "Tests failed              : " << testsFailed << "\n"
        << "\n"
        << "============================================================\n"
        << "Vector Linear Interpolator Test "
        << (allPassed ? "PASS" : "FAIL")
        << "\n"
        << "============================================================\n";

    return allPassed;
}
