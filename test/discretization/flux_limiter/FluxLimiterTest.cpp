#include "discretization/reconstructors/tvd/MinmodLimiter.hpp"
#include "discretization/reconstructors/tvd/SuperbeeLimiter.hpp"
#include "discretization/reconstructors/tvd/VanLeerLimiter.hpp"
#include "discretization/reconstructors/tvd/MCLimiter.hpp"
#include "discretization/reconstructors/tvd/VanAlbadaLimiter.hpp"

#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>

namespace
{

bool nearlyEqual(
    double a,
    double b,
    double tolerance = 1.0e-12
)
{
    return std::abs(a - b) <= tolerance;
}

} // namespace

bool runFluxLimiterTest()
{
    constexpr double tolerance = 1.0e-12;

    MinmodLimiter minmod;
    SuperbeeLimiter superbee;
    VanLeerLimiter vanLeer;
    MCLimiter mc;
    VanAlbadaLimiter vanAlbada;

    bool allPassed = true;
    std::size_t testsPassed = 0;
    std::size_t testsFailed = 0;

    /*
     * ------------------------------------------------------------
     * Non-positive gradient ratios
     * ------------------------------------------------------------
     *
     * For a TVD limiter, opposing gradients should suppress the
     * reconstructed slope.
     *
     *     r <= 0  ->  phi(r) = 0
     *
     * Test several negative values as well as zero.
     */

    {
        const double ratios[] = {
            -2.0,
            -1.0,
            -0.5,
            0.0
        };

        bool passed = true;

        for (double r : ratios)
        {
            passed = passed &&
                     nearlyEqual(minmod.limit(r), 0.0, tolerance) &&
                     nearlyEqual(superbee.limit(r), 0.0, tolerance) &&
                     nearlyEqual(vanLeer.limit(r), 0.0, tolerance) &&
                     nearlyEqual(mc.limit(r), 0.0, tolerance) &&
                     nearlyEqual(vanAlbada.limit(r), 0.0, tolerance);
        }

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Non-positive ratios       : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * r = 1
     * ------------------------------------------------------------
     *
     * All of the implemented TVD limiters should satisfy
     *
     *     phi(1) = 1
     *
     * for a locally linear solution.
     */

    {
        constexpr double r = 1.0;

        const bool passed = nearlyEqual(minmod.limit(r), 1.0, tolerance)
                        &&  nearlyEqual(superbee.limit(r), 1.0, tolerance)
                        &&  nearlyEqual(vanLeer.limit(r), 1.0, tolerance)
                        &&  nearlyEqual(mc.limit(r), 1.0, tolerance)
                        &&  nearlyEqual(vanAlbada.limit(r), 1.0, tolerance);

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Linear region  (r = 1)    : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * r = 0.5
     * ------------------------------------------------------------
     *
     * Expected:
     *
     * Minmod   = 0.5
     * Superbee = 1.0
     * Van Leer = 2/3
     * MC       = 0.75
     * Van Albada = 0.6
     */

    {
        constexpr double r = 0.5;

        constexpr double expectedMinmod = 0.5;
        constexpr double expectedSuperbee = 1.0;
        constexpr double expectedVanLeer = 2.0 / 3.0;
        constexpr double expectedMC = 0.75;
        constexpr double expectedVanAlbada = 0.6;

        const bool passed = nearlyEqual( minmod.limit(r), expectedMinmod, tolerance ) 
                         && nearlyEqual( superbee.limit(r), expectedSuperbee, tolerance ) 
                         && nearlyEqual( vanLeer.limit(r), expectedVanLeer, tolerance ) 
                         && nearlyEqual( mc.limit(r), expectedMC, tolerance ) 
                         && nearlyEqual( vanAlbada.limit(r), expectedVanAlbada, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Limiter values (r = 0.5)  : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * r = 0.25
     * ------------------------------------------------------------
     *
     * Expected:
     *
     * Minmod   = 0.25
     * Superbee = 0.50
     * Van Leer = 0.40
     * MC       = 0.50
     * Van Albada = 0.294117647...
     */

    {
        constexpr double r = 0.25;

        constexpr double expectedMinmod = 0.25;
        constexpr double expectedSuperbee = 0.5;
        constexpr double expectedVanLeer = 0.4;
        constexpr double expectedMC = 0.5;
        constexpr double expectedVanAlbada = (r * r + r) / (r * r + 1.0);

        const bool passed = nearlyEqual( minmod.limit(r), expectedMinmod, tolerance ) 
                         && nearlyEqual( superbee.limit(r), expectedSuperbee, tolerance ) 
                         && nearlyEqual( vanLeer.limit(r), expectedVanLeer, tolerance ) 
                         && nearlyEqual( mc.limit(r), expectedMC, tolerance ) 
                         && nearlyEqual( vanAlbada.limit(r), expectedVanAlbada, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Limiter values (r = 0.25) : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * r = 2
     * ------------------------------------------------------------
     *
     * Expected:
     *
     * Minmod   = 1.0
     * Superbee = 2.0
     * Van Leer = 4/3
     * MC       = 1.5
     * Van Albada = 1.2
     */

    {
        constexpr double r = 2.0;

        constexpr double expectedMinmod = 1.0;
        constexpr double expectedSuperbee = 2.0;
        constexpr double expectedVanLeer = 4.0 / 3.0;
        constexpr double expectedMC = 1.5;
        constexpr double expectedVanAlbada = 1.2;

        const bool passed = nearlyEqual( minmod.limit(r), expectedMinmod, tolerance ) 
                         && nearlyEqual( superbee.limit(r), expectedSuperbee, tolerance ) 
                         && nearlyEqual( vanLeer.limit(r), expectedVanLeer, tolerance ) 
                         && nearlyEqual( mc.limit(r), expectedMC, tolerance ) 
                         && nearlyEqual( vanAlbada.limit(r), expectedVanAlbada, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Limiter values (r = 2.0)  : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Large positive ratio
     * ------------------------------------------------------------
     *
     * This checks the asymptotic/compressive behavior.
     *
     * r = 4:
     *
     * Minmod   = 1
     * Superbee = 2
     * Van Leer = 1.6
     * MC       = 2
     * Van Albada = 20/17
     */

    {
        constexpr double r = 4.0;

        constexpr double expectedMinmod = 1.0;
        constexpr double expectedSuperbee = 2.0;
        constexpr double expectedVanLeer = 1.6;
        constexpr double expectedMC = 2.0;
        constexpr double expectedVanAlbada = (r * r + r) / (r * r + 1.0);

        const bool passed = nearlyEqual( minmod.limit(r), expectedMinmod, tolerance )
                         && nearlyEqual( superbee.limit(r), expectedSuperbee, tolerance )
                         && nearlyEqual( vanLeer.limit(r), expectedVanLeer, tolerance )
                         && nearlyEqual( mc.limit(r), expectedMC, tolerance )
                         && nearlyEqual( vanAlbada.limit(r), expectedVanAlbada, tolerance );

        if (passed) { ++testsPassed; }
        else
        {
            ++testsFailed;
            allPassed = false;
        }

        std::cout << "Limiter values (r = 4.0)  : "
                  << (passed ? "PASS" : "FAIL")
                  << "\n";
    }

    /*
     * ------------------------------------------------------------
     * Summary
     * ------------------------------------------------------------
     */

    std::cout << "\n"
              << "------------------------------------------------------------\n"
              << "Flux Limiter Test Results\n"
              << "------------------------------------------------------------\n"
              << "Tests passed              : " << testsPassed << "\n"
              << "Tests failed              : " << testsFailed << "\n"
              << "\n"
              << "============================================================\n"
              << "Flux Limiter Test "
              << (allPassed ? "PASS" : "FAIL")
              << "\n"
              << "============================================================\n";

    return allPassed;
}
