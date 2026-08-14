#include "discretization/reconstructors/tvd/MinmodLimiter.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

#define TEST_ASSERT(condition) \
    do { \
        if (!(condition)) \
            throw std::runtime_error("Test assertion failed: " #condition); \
    } while (false)

void runMinmodLimiterTest()
{
    MinmodLimiter limiter;

    constexpr double tolerance = 1e-12;

    // -------------------------------------------------
    // Negative ratio
    // -------------------------------------------------

    TEST_ASSERT(std::abs(limiter.limit(-2.0) - 0.0) < tolerance);
    TEST_ASSERT(std::abs(limiter.limit(-0.5) - 0.0) < tolerance);

    // -------------------------------------------------
    // Zero
    // -------------------------------------------------

    TEST_ASSERT(std::abs(limiter.limit(0.0) - 0.0) < tolerance);

    // -------------------------------------------------
    // 0 < r < 1
    //
    // Minmod should return r itself.
    // -------------------------------------------------

    TEST_ASSERT(std::abs(limiter.limit(0.25) - 0.25) < tolerance);
    TEST_ASSERT(std::abs(limiter.limit(0.5) - 0.5) < tolerance);
    TEST_ASSERT(std::abs(limiter.limit(0.75) - 0.75) < tolerance);

    // -------------------------------------------------
    // r = 1
    // -------------------------------------------------

    TEST_ASSERT(std::abs(limiter.limit(1.0) - 1.0) < tolerance);

    // -------------------------------------------------
    // r > 1
    //
    // Minmod should limit the correction to 1.
    // -------------------------------------------------

    TEST_ASSERT(std::abs(limiter.limit(1.5) - 1.0) < tolerance);
    TEST_ASSERT(std::abs(limiter.limit(2.0) - 1.0) < tolerance);
    TEST_ASSERT(std::abs(limiter.limit(10.0) - 1.0) < tolerance);

    // -------------------------------------------------
    // Result
    // -------------------------------------------------

    std::cout << "MinmodLimiter test passed.\n";
}