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
    if (numerical.size() != exact.size()) { throw std::runtime_error("ErrorNorms: size mismatch"); }

    if (numerical.size() != mesh.ncells()) { throw std::runtime_error("ErrorNorms: mesh mismatch"); }

    ErrorNormResults result;

    double l2sum = 0.0;
    double volumeSum = 0.0;
    double linf = 0.0;

    for (std::size_t c = 0; c < mesh.ncells(); ++c)
    {
        const double e = numerical[c] - exact[c];
        const double V = mesh.cellVolume(c);

        l2sum += e * e * V;
        volumeSum += V;

        linf = std::max(linf, std::abs(e));
    }

    result.l2_energy = std::sqrt(l2sum);

    if (volumeSum > 0.0) { result.l2_rms = std::sqrt(l2sum / volumeSum); }

    result.linf = linf;

    return result;
}
