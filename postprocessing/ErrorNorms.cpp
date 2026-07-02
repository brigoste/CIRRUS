#include "postprocessing/ErrorNorms.hpp"

#include <cmath>
#include <stdexcept>

double ErrorNorms::L1(
    const std::vector<double>& numerical,
    const std::vector<double>& exact)
{
    if (numerical.size() != exact.size()) { throw std::runtime_error("Vector size mismatch"); }

    double sum = 0.0;

    for (std::size_t i = 0; i < numerical.size(); ++i) { sum += std::abs(numerical[i] - exact[i]); }

    return sum / numerical.size();
}

double ErrorNorms::L2(
    const std::vector<double>& numerical,
    const std::vector<double>& exact)
{
    if (numerical.size() != exact.size()) { throw std::runtime_error("Vector size mismatch"); }

    double sum = 0.0;

    for (std::size_t i = 0; i < numerical.size(); ++i)
    {
        const double e = numerical[i] - exact[i];
        sum += e * e;
    }

    return std::sqrt(sum / numerical.size());
}

double ErrorNorms::Linf(
    const std::vector<double>& numerical,
    const std::vector<double>& exact)
{
    if (numerical.size() != exact.size()) { throw std::runtime_error("Vector size mismatch"); }

    double maxErr = 0.0;

    for (std::size_t i = 0; i < numerical.size(); ++i)
    {
        maxErr = std::max(
            maxErr,
            std::abs(numerical[i] - exact[i]));
    }

    return maxErr;
}
