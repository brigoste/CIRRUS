#include "discretization/reconstructors/basic/CentralReconstruction.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/QuadMesh2D.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Point.hpp"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <array>

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

double computeOrder(
    double coarseError,
    double fineError
)
{
    return std::log(coarseError / fineError) / std::log(2.0);
}

} // namespace

bool runCentralReconstructionTest()
{
    constexpr double lx = 1.0;
    constexpr double ly = 1.0;
    constexpr std::size_t nx = 10;
    constexpr std::size_t ny = 10;
    constexpr double tolerance = 1.0e-12;

    QuadMesh2D mesh(nx, ny, lx, ly);

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

    /*
     * ------------------------------------------------------------
     * Linear manufactured field
     * ------------------------------------------------------------
     *
     *     phi(x,y) = 2x + 3y + 1
     *
     *     grad(phi) = (2, 3)
     *
     * Central reconstruction should be exact.
     */
    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const Point& center = mesh.cellCenter(c);

        const double x = center[0];
        const double y = center[1];

        phi[c] = 2.0 * x + 3.0 * y + 1.0;

        gradient[c][0] = 2.0;
        gradient[c][1] = 3.0;
    }

    CentralReconstruction reconstruction;

    bool allPassed = true;

    std::size_t facesTested = 0;
    std::size_t stencilFailures = 0;
    std::size_t reconstructionFailures = 0;

    std::cout
        << "\n"
        << "============================================================\n"
        << "Central Reconstruction Test\n"
        << "============================================================\n"
        << "\n"
        << "Manufactured field:\n"
        << "  phi(x,y) = 2x + 3y + 1\n"
        << "  grad(phi) = (2, 3)\n"
        << "\n"
        << "Expected behavior:\n"
        << "  Central reconstruction is exact for a linear field.\n"
        << "\n"
        << "Mesh:\n"
        << "  " << nx << "x" << ny << "\n"
        << "\n"
        << "Tolerance:\n"
        << "  " << std::scientific << tolerance << "\n"
        << "\n";

    /*
     * ------------------------------------------------------------
     * Interior-face stencil and reconstruction test
     * ------------------------------------------------------------
     */
    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor == Face::INVALID)
        {
            continue;
        }

        ++facesTested;

        const std::size_t P = face.owner;

        const Point& xF = face.center;

        const double x = xF[0];
        const double y = xF[1];

        const double exact =
            2.0 * x
            + 3.0 * y
            + 1.0;

        /*
         * Test stencil.
         */
        const ReconstructionStencil stencil =
            reconstruction.stencil(
                mesh,
                P,
                f,
                phi,
                gradient,
                0.0
            );

        double reconstructedFromStencil = 0.0;

        for (const auto& [cell, weight] : stencil.weights)
        {
            reconstructedFromStencil += weight * phi[cell];
        }

        if (!nearlyEqual(
                reconstructedFromStencil,
                exact,
                tolerance))
        {
            ++stencilFailures;
            allPassed = false;
        }

        /*
         * Test direct reconstruction.
         */
        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                P,
                f,
                phi,
                gradient,
                0.0
            );

        if (!nearlyEqual(
                reconstructed,
                exact,
                tolerance))
        {
            ++reconstructionFailures;
            allPassed = false;
        }
    }

    /*
     * ------------------------------------------------------------
     * Boundary-face rejection
     * ------------------------------------------------------------
     */
    bool boundaryPassed = false;

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor != Face::INVALID)
        {
            continue;
        }

        const std::size_t P = face.owner;

        try
        {
            reconstruction.stencil(
                mesh,
                P,
                f,
                phi,
                gradient,
                0.0
            );
        }
        catch (const std::runtime_error&)
        {
            boundaryPassed = true;
        }

        break;
    }

    if (!boundaryPassed)
    {
        allPassed = false;
    }

    /*
     * ------------------------------------------------------------
     * Linear test results
     * ------------------------------------------------------------
     */
    std::cout
        << "------------------------------------------------------------\n"
        << "Interior Faces\n"
        << "------------------------------------------------------------\n"
        << "Faces tested              : " << facesTested << "\n"
        << "Stencil failures          : " << stencilFailures << "\n"
        << "Reconstruction failures   : " << reconstructionFailures << "\n"
        << "\n"
        << "Boundary-face rejection   : "
        << (boundaryPassed ? "PASS" : "FAIL")
        << "\n"
        << "\n";

    /*
     * ------------------------------------------------------------
     * Quadratic refinement test
     * ------------------------------------------------------------
     *
     *     phi(x,y) = x^2 + y^2
     *
     *     grad(phi) = (2x, 2y)
     *
     * Central reconstruction is expected to be second-order
     * accurate because the neglected Taylor terms are O(h^2).
     */
    struct RefinementResult
    {
        std::size_t nx;
        double h;
        double error;
    };

    constexpr std::array<std::size_t, 4> refinementLevels = { 10, 20, 40, 80 };

    constexpr double minimumOrder = 1.8;

    std::vector<RefinementResult> refinementResults;
    refinementResults.reserve(refinementLevels.size());

    std::cout
        << "============================================================\n"
        << "Central Reconstruction Refinement Test\n"
        << "============================================================\n"
        << "\n"
        << "Manufactured field:\n"
        << "  phi(x,y) = x^2 + y^2\n"
        << "  grad(phi) = (2x, 2y)\n"
        << "\n"
        << "Expected behavior:\n"
        << "  Second-order convergence under mesh refinement.\n"
        << "\n"
        << "Minimum accepted order:\n"
        << "  " << std::fixed << std::setprecision(2)
        << minimumOrder << "\n"
        << "\n";

    for (const std::size_t level : refinementLevels)
    {
        QuadMesh2D refinementMesh(
            level,
            level,
            lx,
            ly
        );

        ScalarField refinementPhi(
            "phi",
            refinementMesh,
            FieldLocation::Cell
        );

        VectorField refinementGradient(
            "gradient",
            refinementMesh,
            FieldLocation::Cell
        );

        for (std::size_t c = 0;
             c < refinementMesh.ncells();
             ++c)
        {
            const Point& center =
                refinementMesh.cellCenter(c);

            const double x = center[0];
            const double y = center[1];

            refinementPhi[c] =
                x * x + y * y;

            refinementGradient[c][0] =
                2.0 * x;

            refinementGradient[c][1] =
                2.0 * y;
        }

        double error = 0.0;
        double reference = 0.0;

        for (std::size_t f = 0;
             f < refinementMesh.nfaces();
             ++f)
        {
            const Face& face =
                refinementMesh.face(f);

            if (face.neighbor == Face::INVALID)
            {
                continue;
            }

            const std::size_t P = face.owner;

            const ReconstructionStencil stencil =
                reconstruction.stencil(
                    refinementMesh,
                    P,
                    f,
                    refinementPhi,
                    refinementGradient,
                    0.0
                );

            double reconstructed = 0.0;

            for (const auto& [cell, weight] : stencil.weights)
            {
                reconstructed +=
                    weight * refinementPhi[cell];
            }

            const double x = face.center[0];
            const double y = face.center[1];

            const double exact =
                x * x + y * y;

            const double difference =
                reconstructed - exact;

            error += difference * difference;
            reference += exact * exact;
        }

        const double l2Error =
            std::sqrt(error / reference);

        const double h =
            lx / static_cast<double>(level);

        refinementResults.push_back(
            {
                level,
                h,
                l2Error
            }
        );
    }

    std::cout
        << "------------------------------------------------------------\n"
        << "Mesh Refinement\n"
        << "------------------------------------------------------------\n";

    std::cout
        << std::left
        << std::setw(12) << "Mesh"
        << std::setw(16) << "h"
        << std::setw(20) << "L2 Error"
        << "\n";

    std::cout
        << "------------------------------------------------------------\n";

    for (const auto& result : refinementResults)
    {
        std::cout
            << std::left
            << std::setw(12)
            << (
                std::to_string(result.nx)
                + "x"
                + std::to_string(result.nx)
            )
            << std::scientific
            << std::setprecision(6)
            << std::setw(16)
            << result.h
            << std::setw(20)
            << result.error
            << "\n";
    }

    std::cout
        << "\n"
        << "------------------------------------------------------------\n"
        << "Observed Order\n"
        << "------------------------------------------------------------\n";

    std::cout
        << std::left
        << std::setw(20) << "Refinement"
        << std::setw(20) << "Order"
        << "\n";

    std::cout
        << "------------------------------------------------------------\n";

    for (std::size_t i = 1;
         i < refinementResults.size();
         ++i)
    {
        const double order =
            computeOrder(
                refinementResults[i - 1].error,
                refinementResults[i].error
            );

        const bool passed =
            order >= minimumOrder;

        if (!passed)
        {
            allPassed = false;
        }

        std::cout
            << std::left
            << std::setw(20)
            << (
                std::to_string(refinementResults[i - 1].nx)
                + " -> "
                + std::to_string(refinementResults[i].nx)
            )
            << std::fixed
            << std::setprecision(4)
            << std::setw(20)
            << order
            << (passed ? "PASS" : "FAIL")
            << "\n";
    }

    std::cout
        << "\n"
        << "============================================================\n"
        << "Central Reconstruction Test "
        << (allPassed ? "PASS" : "FAIL")
        << "\n"
        << "============================================================\n";

    return allPassed;
}
