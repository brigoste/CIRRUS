#include "tests/verification/VerificationRunner.hpp"

#include "tests/verification/VerificationCase.hpp"
#include "tests/verification/ErrorMetrics.hpp"
#include "mesh/MeshBase.hpp"

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <algorithm>

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
    const VerificationCase& verificationCase,
    const VerificationConfig& config)
{
    if (!config.enabled)
        return;

    if (solution.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "VerificationRunner: solution size does not match mesh cells");
    }

    // -------------------------------------------------
    // 1. Exact field
    // -------------------------------------------------
    auto exact = buildExactField(mesh, verificationCase);

    // ================= DEBUG: spot-check PDE consistency =================
    std::size_t c = 0; // pick first or middle cell
    const auto& xc = mesh.cellCenter(c);

    double x = xc.x[0];
    double y = xc.x[1];

    double phi_exact = exact[c];
    double rhs_exact  = verificationCase.source(x, y);

    std::cout << "\n================ DEBUG CELL CHECK ================\n";
    std::cout << "Cell index: " << c << "\n";
    std::cout << "x = " << x << ", y = " << y << "\n";
    std::cout << "phi_exact = " << phi_exact << "\n";
    std::cout << "source(x,y) = " << rhs_exact << "\n";
    std::cout << "=================================================\n\n";

    // -------------------------------------------------
    // 2. Error field
    // -------------------------------------------------
    auto error = buildErrorField(solution, exact);

    // -------------------------------------------------
    // 3. Norms
    // -------------------------------------------------
    const ErrorNormResults norms =
        ErrorNorms::compute(mesh, solution, exact);

    // -------------------------------------------------
    // 4. Output summary
    // -------------------------------------------------
    printSummary(norms);

    // -------------------------------------------------
    // 5. Optional: lightweight diagnostics (useful for debugging)
    // -------------------------------------------------
#ifdef DEBUG
    const auto [emin, emax] =
        std::minmax_element(error.begin(), error.end());

    std::cout << "[DEBUG] Error range: "
              << *emin << " to " << *emax << "\n";
#endif
}
