#include "tests/verification/ManufacturedCases/SinusoidalCase.hpp"
#include "SinusoidalCase.hpp"
#include "tests/verification/VerificationRegistry.hpp"

#include <cmath>

static constexpr double PI = 3.14159265358979323846;

namespace
{
    const bool registered = []()
    {
        VerificationRegistry::instance().registerCase(
            "sinusoidal",
            [](const VerificationConfig&)
            {
                return std::make_unique<SinusoidalCase>();
            });

        return true;
    }();
}

double SinusoidalCase::exact(
    double x,
    double y) const
{
    return std::sin(PI * x) * std::sin(PI * y);
}

double SinusoidalCase::source(
    double x,
    double y) const
{
    return 2.0 * PI * PI *
           std::sin(PI * x) *
           std::sin(PI * y);
}
