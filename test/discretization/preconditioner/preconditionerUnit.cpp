#include "equation_systems/LinearSystem.hpp"
#include "solver/preconditioners/SSORPreconditioner.hpp"

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

constexpr double tolerance = 1.0e-12;

bool approxEqual(double a, double b)
{
    return std::abs(a - b) <= tolerance;
}

void assertVectorEqual(
    const std::vector<double>& actual,
    const std::vector<double>& expected)
{
    if (actual.size() != expected.size()) { throw std::runtime_error("Vector size mismatch"); }

    for (std::size_t i = 0; i < actual.size(); ++i)
    {
        if (!approxEqual(actual[i], expected[i]))
        {
            throw std::runtime_error("Vector mismatch at index "
                                    + std::to_string(i)
                                    + ": actual = "
                                    + std::to_string(actual[i])
                                    + ", expected = "
                                    + std::to_string(expected[i]));
        }
    }
}

LinearSystem createTestSystem()
{
    LinearSystem system(3);

    system.addCoeff(0, 0, 4.0);
    system.addCoeff(0, 1, -1.0);

    system.addCoeff(1, 0, -1.0);
    system.addCoeff(1, 1, 4.0);
    system.addCoeff(1, 2, -1.0);

    system.addCoeff(2, 1, -1.0);
    system.addCoeff(2, 2, 4.0);

    return system;
}
LinearSystem createNonsymmetricTestSystem()
{
    LinearSystem system(3);

    system.addCoeff(0, 0, 4.0);
    system.addCoeff(0, 1, -2.0);

    system.addCoeff(1, 0, -1.0);
    system.addCoeff(1, 1, 5.0);
    system.addCoeff(1, 2, -2.0);

    system.addCoeff(2, 1, -1.0);
    system.addCoeff(2, 2, 3.0);

    return system;
}

std::vector<double> applySSORMatrix(
    const LinearSystem& system,
    double omega,
    const std::vector<double>& z)
{
    const std::size_t n = system.size();

    std::vector<std::vector<double>> M( n,
                                        std::vector<double>(n, 0.0));

    // --------------------------------------------------------
    // Construct:
    //
    // M = 1/[omega(2-omega)]
    //     (D + omega L) D^-1 (D + omega U)
    //
    // explicitly for the test.
    // --------------------------------------------------------

    const double scale = 1.0 / (omega * (2.0 - omega));

    // Build the two triangular matrices.
    std::vector<std::vector<double>> lower( n,
                                            std::vector<double>(n, 0.0));

    std::vector<std::vector<double>> upper( n,
                                            std::vector<double>(n, 0.0));

    for (std::size_t i = 0; i < n; ++i)
    {
        for (std::size_t j = 0; j < n; ++j)
        {
            const double aij = system.coeff(i, j);

            if (i == j)
            {
                lower[i][j] = aij;
                upper[i][j] = aij;
            }
            else if (j < i) { lower[i][j] = omega * aij; }
            else            { upper[i][j] = omega * aij; }
        }
    }

    // Compute:
    //
    //     M = scale * Lw * D^-1 * Uw
    //
    std::vector<std::vector<double>> Mscaled( n,
                                              std::vector<double>(n, 0.0));

    for (std::size_t i = 0; i < n; ++i)
    {
        for (std::size_t j = 0; j < n; ++j)
        {
            for (std::size_t k = 0; k < n; ++k)
            {
                Mscaled[i][j] += lower[i][k] * (upper[k][j] / system.diagonal(k));
            }

            M[i][j] = scale * Mscaled[i][j];
        }
    }

    std::vector<double> result(n, 0.0);

    for (std::size_t i = 0; i < n; ++i)
    {
        for (std::size_t j = 0; j < n; ++j)
        {
            result[i] += M[i][j] * z[j];
        }
    }

    return result;
}

void testSSORApplication()
{
    const std::vector<double> rhs = {
        1.0,
        2.0,
        3.0
    };

    for (const double omega : {0.5, 1.0, 1.2, 1.8})
    {
        LinearSystem system = createTestSystem();

        SSORPreconditioner preconditioner(omega);
        preconditioner.setup(system);

        std::vector<double> z;

        preconditioner.apply(rhs, z);

        const std::vector<double> reconstructed = applySSORMatrix(system, omega, z);

        assertVectorEqual(reconstructed, rhs);

        std::cout << "omega = "
                  << omega
                  << " : PASS\n";
    }
}

void testSSORInvalidOmega()
{
    std::cout << "\n---- SSOR Invalid Omega Test ----\n";

    bool passed = true;

    for (const double omega : {0.0, -1.0, 2.0, 3.0})
    {
        try
        {
            SSORPreconditioner preconditioner(omega);
            passed = false;
        }
        catch (const std::invalid_argument&) {}
    }

    if (!passed) { throw std::runtime_error( "Invalid omega was not rejected"); }

    std::cout << "Invalid omega : PASS\n";
}

void testSSORZeroDiagonal()
{
    std::cout << "\n---- SSOR Zero Diagonal Test ----\n";

    LinearSystem system(2);

    system.addCoeff(0, 0, 1.0);
    system.addCoeff(1, 1, 0.0);

    SSORPreconditioner preconditioner;

    bool threw = false;

    try                               { preconditioner.setup(system); }
    catch (const std::runtime_error&) { threw = true; }

    if (!threw) { throw std::runtime_error("Zero diagonal was not rejected"); }

    std::cout << "Zero diagonal : PASS\n";
}
void testSSORSGSEquivalence()
{
    std::cout << "\n---- SSOR / SGS Equivalence Test ----\n";

    LinearSystem system = createTestSystem();

    const std::vector<double> rhs = {
        1.0,
        2.0,
        3.0
    };

    SSORPreconditioner ssor(1.0);
    ssor.setup(system);

    std::vector<double> zSSOR;
    ssor.apply(rhs, zSSOR);

    // --------------------------------------------------------
    // SGS:
    //
    //     (D + L)y = r
    //     w = D y
    //     (D + U)z = w
    //
    // --------------------------------------------------------

    const std::size_t n = system.size();

    std::vector<double> y(n, 0.0);

    for (std::size_t i = 0; i < n; ++i)
    {
        double value = rhs[i];

        for (const auto& [j, aij] : system.row(i))
        {
            if (j < i) { value -= aij * y[j]; }
        }

        y[i] = value / system.diagonal(i);
    }

    std::vector<double> w(n, 0.0);

    for (std::size_t i = 0; i < n; ++i)
    {
        w[i] = system.diagonal(i) * y[i];
    }

    std::vector<double> zSGS(n, 0.0);

    for (std::size_t i = n; i-- > 0;)
    {
        double value = w[i];

        for (const auto& [j, aij] : system.row(i))
        {
            if (j > i) { value -= aij * zSGS[j]; }
        }

        zSGS[i] = value / system.diagonal(i);
    }

    assertVectorEqual(zSSOR, zSGS);

    std::cout << "SSOR omega = 1 == SGS : PASS\n";
}
void testSSORNonsymmetric()
{
    std::cout << "\n---- SSOR Nonsymmetric Matrix Test ----\n";

    LinearSystem system = createNonsymmetricTestSystem();

    const std::vector<double> rhs = {
        1.0,
        2.0,
        3.0
    };

    for (const double omega : {0.5, 1.0, 1.2, 1.8})
    {
        SSORPreconditioner preconditioner(omega);
        preconditioner.setup(system);

        std::vector<double> z;

        preconditioner.apply(rhs, z);

        const std::vector<double> reconstructed = applySSORMatrix(system, omega, z);

        assertVectorEqual(reconstructed, rhs);

        std::cout << "omega = "
                  << omega
                  << " : PASS\n";
    }
}
