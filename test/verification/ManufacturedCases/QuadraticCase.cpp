#include "tests/verification/ManufacturedCases/QuadraticCase.hpp"
#include "tests/verification/VerificationAutoRegister.hpp"
#include "tests/verification/VerificationRegistry.hpp"

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

static VerificationAutoRegister register_quadratic(
    "quadratic",
    [](const VerificationConfig&)
    {
        return std::make_unique<QuadraticCase>();
    });
