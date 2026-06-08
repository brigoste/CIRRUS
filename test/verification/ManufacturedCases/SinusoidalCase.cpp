#include "tests/verification/ManufacturedCases/SinusoidalCase.hpp"
#include "tests/verification/VerificationAutoRegister.hpp"
#include "utils/MathConstants.hpp"
#define _USE_MATH_DEFINES
#include <cmath>


// ---------------------------
// Case implementation
// ---------------------------

double SinusoidalCase::exact(double x, double y) const
{
    using math::PI;
    return std::sin(PI * x) * std::sin(PI * y);
}

double SinusoidalCase::source(double x, double y) const
{
    return 2.0 * math::PI2 * std::sin(math::PI * x) * std::sin(math::PI * y);
    // return 999.0;
}

// ---------------------------
// Self-registration
// ---------------------------

static VerificationAutoRegister register_sinusoidal(
    "sinusoidal",
    [](const VerificationConfig&)
    {
        return std::make_unique<SinusoidalCase>();
    });
