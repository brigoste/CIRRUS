#include "tests/verification/VerificationRunner.hpp"

#include "tests/verification/VerificationCase.hpp"
#include "tests/verification/ErrorMetrics.hpp"
#include "mesh/MeshBase.hpp"

#include <iostream>
#include <stdexcept>
#include <cmath>

std::vector<double>
VerificationRunner::buildExactField(
    const MeshBase& mesh,
    const VerificationCase& verificationCase)
{
    const std::size_t n = mesh.ncells();

    std::vector<double> exact;
    exact.resize(n);

    for (std::size_t c = 0; c < n; ++c)
    {
        const auto& xc = mesh.cellCenter(c);

        double x = xc.x[0];
        double y = xc.x[1];

        exact[c] = verificationCase.exact(x, y);
    }

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
            "VerificationRunner: size mismatch");
    }

    std::vector<double> error(numerical.size());

    for (std::size_t i = 0; i < numerical.size(); ++i)
    {
        error[i] = numerical[i] - exact[i];
    }

    return error;
}

void VerificationRunner::printSummary(
    const ErrorNormResults& norms)
{
    std::cout << "\n";
    std::cout << "================ VERIFICATION ================\n";
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

    // -----------------------------------------
    // 1. Exact solution field
    // -----------------------------------------
    auto exact =
        buildExactField(mesh, verificationCase);

    // -----------------------------------------
    // 2. Error field
    // -----------------------------------------
    auto error =
        buildErrorField(solution, exact);

    // -----------------------------------------
    // 3. Norms
    // -----------------------------------------
    ErrorNormResults norms =
        ErrorNorms::compute(
            mesh,
            solution,
            exact);

    // -----------------------------------------
    // 4. Output
    // -----------------------------------------
    printSummary(norms);

    // (future extension: CSV/JSON export)
}
