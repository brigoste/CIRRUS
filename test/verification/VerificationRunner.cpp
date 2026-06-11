#include "tests/verification/VerificationRunner.hpp"

#include "tests/verification/VerificationCase.hpp"
#include "tests/verification/ErrorMetrics.hpp"
#include "mesh/MeshBase.hpp"

#include "physics/PhysicsModel.hpp"

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>

// #error "VerificationRunner.cpp is being compiled"

std::vector<double>
VerificationRunner::buildExactField(
    const MeshBase& mesh,
    const VerificationCase& verificationCase)
{
    const std::size_t n = mesh.ncells();

    std::cout << "\n=================== Building Field ===================\n";

    std::vector<double> exact(n);

    for (std::size_t c = 0; c < n; ++c)
    {
        const auto& xc = mesh.cellCenter(c);

        const double x = xc.x[0];
        const double y = xc.x[1];

        exact[c] = verificationCase.exact(x, y);
    }

    const auto [minIt, maxIt] =
        std::minmax_element(exact.begin(), exact.end());

    std::cout << "Exact min = " << *minIt << "\n";
    std::cout << "Exact max = " << *maxIt << "\n";

    return exact;
}

std::vector<double>
VerificationRunner::buildErrorField(
    const std::vector<double>& numerical,
    const std::vector<double>& exact)
{
    if (numerical.size() != exact.size())
    {
        throw std::runtime_error(
            "VerificationRunner: size mismatch between numerical and exact fields");
    }

    const std::size_t n = numerical.size();
    std::vector<double> error(n);

    for (std::size_t i = 0; i < n; ++i)
    {
        error[i] = numerical[i] - exact[i];
    }

    return error;
}

void VerificationRunner::printSummary(
    const ErrorNormResults& norms)
{
    std::cout << "\n================ VERIFICATION ================\n";
    std::cout << "L2 Norm   : " << norms.l2 << "\n";
    std::cout << "Linf Norm : " << norms.linf << "\n";
    std::cout << "=============================================\n\n";
}

void VerificationRunner::run(
    const MeshBase& mesh,
    const std::vector<double>& solution,
    const LinearSystem& sys,
    const VerificationCase& verificationCase,
    const VerificationConfig& config)
{
    std::cout << "============== Running Verification ==================\n\n";

    if (!config.enabled)
        return;

    // -------------------------------------------------
    // Sanity checks
    // -------------------------------------------------
    if (solution.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "VerificationRunner: solution size does not match mesh");
    }

    if (sys.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "VerificationRunner: system size does not match mesh");
    }

    // -------------------------------------------------
    // Build exact field
    // -------------------------------------------------
    auto exact = buildExactField(mesh, verificationCase);

    // -------------------------------------------------
    // Compute error field
    // -------------------------------------------------
    auto error = buildErrorField(solution, exact);

    // -------------------------------------------------
    // Compute norms
    // -------------------------------------------------
    const ErrorNormResults norms = ErrorNorms::compute(mesh, solution, exact);

    // -------------------------------------------------
    // Discrete operator audit
    // -------------------------------------------------
    std::size_t cell = std::min<std::size_t>(25, mesh.ncells() - 1);

    const auto& xc = mesh.cellCenter(cell);

    double lhs = 0.0;

    for (std::size_t j = 0; j < sys.size(); ++j)
    {
        lhs += sys.coeff(cell, j) * exact[j];
    }

    double rhs = sys.rhs(cell);
    double residual = lhs - rhs;

    std::cout
        << "\n================ DISCRETE AUDIT ================\n"
        << "Cell      : " << cell << "\n"
        << "x         : " << xc.x[0] << "\n"
        << "y         : " << xc.x[1] << "\n"
        << "\n"
        << "phi_exact : " << exact[cell] << "\n"
        << "phi_num   : " << solution[cell] << "\n"
        << "error     : "
        << solution[cell] - exact[cell]
        << "\n\n"
        << "LHS       : " << lhs << "\n"
        << "RHS       : " << rhs << "\n"
        << "Residual  : " << residual << "\n"
        << "===============================================\n";

    std::cout << "\nRow coefficients:\n";

    for (std::size_t j = 0; j < sys.size(); ++j)
    {
        double a = sys.coeff(cell, j);

        if (std::abs(a) > 1e-12)
        {
            std::cout << "A[" << cell << "," << j << "] = " << a << "\n";
        }
    }

    std::cout << "\nExact stencil values:\n";

    for (std::size_t j = 0; j < sys.size(); ++j)
    {
        double a = sys.coeff(cell, j);

        if (std::abs(a) > 1e-12)
        {
            std::cout << "phi[" << j << "] = " << exact[j] << "\n";
        }
    }

    double volume = mesh.cellVolume(cell);

    std::cout
        << "Volume      : " << volume << "\n"
        << "Source raw  : "
        << verificationCase.source(
            xc.x[0],
            xc.x[1])
        << "\n"
        << "Source*V    : "
        << verificationCase.source(
            xc.x[0],
            xc.x[1]) * volume
        << "\n";

    // -------------------------------------------------
    // Verification summary
    // -------------------------------------------------
    printSummary(norms);
}

#ifdef DEBUG
    const auto [emin, emax] =
        std::minmax_element(error.begin(), error.end());

    std::cout << "[DEBUG] Error range: "
              << *emin << " to " << *emax << "\n";
#endif
