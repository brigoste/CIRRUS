#include "discretization/gradient/GreenGaussGradient.hpp"
#include "discretization/gradient/LeastSquaresGradient.hpp"
#include "discretization/reconstructors/GradientReconstruction.hpp"

#include "fields/ScalarField.hpp"
#include "fields/VectorField.hpp"

#include "mesh/QuadMesh2D.hpp"
#include "mesh/primitives/Face.hpp"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

struct RefinementResult
{
    int nx;
    double h;
    double ggError;
    double lsError;
};

namespace
{

double computeFaceL2Error(
    const MeshBase& mesh,
    const ScalarField& field,
    const VectorField& gradient,
    const GradientReconstruction& reconstruction
)
{
    double error = 0.0;
    double reference = 0.0;

    for (std::size_t f = 0; f < mesh.nfaces(); ++f)
    {
        const Face& face = mesh.face(f);

        if (face.neighbor == Face::INVALID) { continue; }

        const std::size_t P = face.owner;

        const double reconstructed =
            reconstruction.reconstruct(
                mesh,
                P,
                face,
                field,
                gradient,
                0.0
            );

        const double x = face.center[0];
        const double y = face.center[1];

        const double exact =
            x * x * x
            + y * y * y;

        const double diff = reconstructed - exact;

        error += diff * diff;
        reference += exact * exact;
    }

    return std::sqrt(error / reference);
}

double computeOrder(
    double coarseError,
    double fineError
)
{
    return std::log(coarseError / fineError) / std::log(2.0);
}

} // namespace

bool runGradientReconstructionTest()
{
    constexpr double lx = 1.0;
    constexpr double ly = 1.0;

    constexpr double tolerance = 1.0e-2;

    const std::vector<std::size_t> refinementLevels =
    {
        20,
        40,
        80,
        160
    };

    GreenGaussGradient greenGauss;
    LeastSquaresGradient leastSquares;
    GradientReconstruction reconstruction;

    std::vector<RefinementResult> results;
    results.reserve(refinementLevels.size());

    bool allPassed = true;

    std::cout
        << "\n"
        << "============================================================\n"
        << "Gradient Reconstruction Verification\n"
        << "============================================================\n"
        << "\n"
        << "Manufactured field:\n"
        << "  phi(x,y) = x^3 + y^3\n"
        << "  grad(phi) = (3x^2, 3y^2)\n"
        << "\n"
        << "Reconstruction:\n"
        << "  phi_f = phi_P + grad(phi)_P . (x_f - x_P)\n"
        << "\n"
        << "Error metric:\n"
        << "  Normalized L2 error over interior faces only\n"
        << "\n"
        << "Acceptance tolerance:\n"
        << "  Error < " << std::scientific << tolerance << "\n"
        << "\n"
        << "------------------------------------------------------------\n"
        << "Mesh Refinement\n"
        << "------------------------------------------------------------\n";

    std::cout
        << std::left
        << std::setw(14) << "Mesh"
        << std::setw(16) << "h"
        << std::setw(24) << "Green-Gauss"
        << std::setw(24) << "Least-Squares"
        << "\n";

    std::cout
        << "------------------------------------------------------------\n";

    for (const std::size_t nx : refinementLevels)
    {
        QuadMesh2D mesh(nx, nx, lx, ly);

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

            phi[c] =
                x * x * x
                + y * y * y;
        }

        greenGauss.compute(
            mesh,
            phi,
            ggGradient
        );

        leastSquares.compute(
            mesh,
            phi,
            lsGradient
        );

        const double ggError =
            computeFaceL2Error(
                mesh,
                phi,
                ggGradient,
                reconstruction
            );

        const double lsError =
            computeFaceL2Error(
                mesh,
                phi,
                lsGradient,
                reconstruction
            );

        const bool ggPassed = ggError < tolerance;
        const bool lsPassed = lsError < tolerance;

        allPassed = allPassed && ggPassed && lsPassed;

        const double h =
            lx / static_cast<double>(nx);

        results.push_back(
            {
                static_cast<int>(nx),
                h,
                ggError,
                lsError
            }
        );

        std::cout
            << std::left
            << std::setw(14)
            << (std::to_string(nx) + "x" + std::to_string(nx))
            << std::scientific
            << std::setprecision(6)
            << std::setw(16)
            << h
            << std::setw(24)
            << (std::to_string(ggError)
                + (ggPassed ? " PASS" : " FAIL"))
            << std::setw(24)
            << (std::to_string(lsError)
                + (lsPassed ? " PASS" : " FAIL"))
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
        << std::setw(20) << "Green-Gauss"
        << std::setw(20) << "Least-Squares"
        << "\n";

    std::cout
        << "------------------------------------------------------------\n";

    for (std::size_t i = 1; i < results.size(); ++i)
    {
        const double ggOrder =
            computeOrder(
                results[i - 1].ggError,
                results[i].ggError
            );

        const double lsOrder =
            computeOrder(
                results[i - 1].lsError,
                results[i].lsError
            );

        std::cout
            << std::left
            << std::setw(20)
            << (
                std::to_string(results[i - 1].nx)
                + " -> "
                + std::to_string(results[i].nx)
            )
            << std::fixed
            << std::setprecision(4)
            << std::setw(20)
            << ggOrder
            << std::setw(20)
            << lsOrder
            << "\n";
    }

    std::cout
        << "\n"
        << "============================================================\n"
        << "Gradient Reconstruction Verification "
        << (allPassed ? "PASS" : "FAIL")
        << "\n"
        << "============================================================\n";

    return allPassed;
}
