#include "tests/verification/ManufacturedCases/QuadraticCase.hpp"
#include "tests/verification/VerificationRegistry.hpp"

namespace
{
    const bool registered = []()
    {
        VerificationRegistry::instance().registerCase(
            "quadratic",
            [](const VerificationConfig&)
            {
                return std::make_unique<QuadraticCase>();
            });

        return true;
    }();
}

double QuadraticCase::exact(
    double x,
    double y) const
{
    return x*x + y*y;
}

double QuadraticCase::source(
    double,
    double) const
{
    return -4.0;
}
