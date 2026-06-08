#include "tests/verification/ErrorMetrics.hpp"
#include "mesh/MeshBase.hpp"

#include <stdexcept>
#include <cmath>
#include <algorithm>

ErrorNormResults ErrorNorms::compute(
    const MeshBase& mesh,
    const std::vector<double>& numerical,
    const std::vector<double>& exact)
{
    if (numerical.size() != exact.size())
    {
        throw std::runtime_error(
            "ErrorNorms: numerical/exact size mismatch");
    }

    if (numerical.size() != mesh.ncells())
    {
        throw std::runtime_error(
            "ErrorNorms: field size does not match mesh");
    }

    ErrorNormResults result;

    double l2sum = 0.0;
    double volumeSum = 0.0;
    double linf = 0.0;

    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const double error =
            numerical[c] - exact[c];

        const double V =
            mesh.cellVolume(c);

        l2sum += error * error * V;

        volumeSum += V;

        linf = std::max(
            linf,
            std::abs(error));
    }

    if (volumeSum > 0.0)
    {
        result.l2 =
            std::sqrt(l2sum / volumeSum);
    }

    result.linf = linf;

    return result;
}
