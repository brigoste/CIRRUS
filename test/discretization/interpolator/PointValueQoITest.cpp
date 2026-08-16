#include "verification/qoi/PointValueQoI.hpp"

#include "fields/ScalarField.hpp"

#include "interpolators/LinearInterpolator.hpp"

#include "mesh/Mesh1D.hpp"
#include "mesh/primitives/Point.hpp"

#include <cmath>
#include <cstddef>
#include <iostream>

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

void runPointValueQoITest()
{
    constexpr double length = 1.0;
    constexpr std::size_t nCells = 10;
    constexpr double tolerance = 1.0e-12;

    Mesh1D mesh(
        nCells,
        length
    );

    ScalarField phi(
        "phi",
        mesh,
        FieldLocation::Node
    );

    /*
     * ------------------------------------------------------------
     * Manufactured field
     * ------------------------------------------------------------
     *
     *     phi(x) = 2x + 1
     *
     * Linear interpolation should reproduce this field exactly.
     */
    for (std::size_t i = 0; i < mesh.nnodes(); ++i)
    {
        const double x = mesh.node(i)[0];

        phi[i] = 2.0 * x + 1.0;
    }

    const Point position({
        0.375,
        0.0,
        0.0
    });

    const double exact = 2.0 * position[0] + 1.0;

    LinearInterpolator interpolator;

    PointValueQoI qoi(position);

    bool allPassed = true;

    std::size_t testsPassed = 0;
    std::size_t testsFailed = 0;

    std::cout
        << "\n"
        << "============================================================\n"
        << "Point Value QoI Test\n"
        << "============================================================\n"
        << "\n"
        << "Manufactured field:\n"
        << "  phi(x) = 2x + 1\n"
        << "\n"
        << "Evaluation point:\n"
        << "  x = " << position[0] << "\n"
        << "\n"
        << "Expected value:\n"
        << "  phi(x) = " << exact << "\n"
        << "\n"
        << "Tolerance:\n"
        << "  " << std::scientific << tolerance << "\n"
        << "\n";

    /*
     * ------------------------------------------------------------
     * QoI evaluation
     * ------------------------------------------------------------
     */
    const double value =
        qoi.evaluate(
            phi,
            interpolator
        );

    const bool evaluationPassed =
        nearlyEqual(
            value,
            exact,
            tolerance
        );

    if (evaluationPassed) { ++testsPassed; }
    else
    {
        ++testsFailed;
        allPassed = false;
    }

    std::cout
        << "QoI evaluation            : "
        << (evaluationPassed ? "PASS" : "FAIL")
        << "\n";

    /*
     * ------------------------------------------------------------
     * Position
     * ------------------------------------------------------------
     */
    const Point& storedPosition =
        qoi.position();

    const bool positionPassed =
        nearlyEqual(
            storedPosition[0],
            position[0],
            tolerance
        )
        &&
        nearlyEqual(
            storedPosition[1],
            position[1],
            tolerance
        )
        &&
        nearlyEqual(
            storedPosition[2],
            position[2],
            tolerance
        );

    if (positionPassed) { ++testsPassed; }
    else
    {
        ++testsFailed;
        allPassed = false;
    }

    std::cout
        << "Position storage          : "
        << (positionPassed ? "PASS" : "FAIL")
        << "\n";

    /*
     * ------------------------------------------------------------
     * Summary
     * ------------------------------------------------------------
     */
    std::cout
        << "\n"
        << "------------------------------------------------------------\n"
        << "Point Value QoI Results\n"
        << "------------------------------------------------------------\n"
        << "Tests passed              : " << testsPassed << "\n"
        << "Tests failed              : " << testsFailed << "\n"
        << "\n"
        << "============================================================\n"
        << "Point Value QoI Test "
        << (allPassed ? "PASS" : "FAIL")
        << "\n"
        << "============================================================\n";
}