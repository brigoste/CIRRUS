#pragma once

#include "linear_system/LinearSystem.hpp"
#include <vector>
#include <cmath>
#include <stdexcept>

namespace LA
{

// --------------------------------------------------
// Matrix-vector product: y = A x
// --------------------------------------------------
inline void matvec(const LinearSystem& sys,
                   const std::vector<double>& x,
                   std::vector<double>& y)
{
    const int N = sys.size();

    if ((int)x.size() != N || (int)y.size() != N)
        throw std::runtime_error("matvec: dimension mismatch");

    for (int i = 0; i < N; ++i)
    {
        double sum = sys.diag(i) * x[i];

        for (const auto& [j, aij] : sys.row(i))
        {
            sum += aij * x[j];
        }

        y[i] = sum;
    }
}

// --------------------------------------------------
// Dot product
// --------------------------------------------------
inline double dot(const std::vector<double>& a,
                  const std::vector<double>& b)
{
    if (a.size() != b.size())
        throw std::runtime_error("dot: size mismatch");

    double s = 0.0;

    for (size_t i = 0; i < a.size(); ++i)
        s += a[i] * b[i];

    return s;
}

// --------------------------------------------------
// y += alpha * x
// --------------------------------------------------
inline void axpy(double alpha,
                 const std::vector<double>& x,
                 std::vector<double>& y)
{
    if (x.size() != y.size())
        throw std::runtime_error("axpy: size mismatch");

    for (size_t i = 0; i < x.size(); ++i)
        y[i] += alpha * x[i];
}

// --------------------------------------------------
// L2 norm
// --------------------------------------------------
inline double norm2(const std::vector<double>& x)
{
    double s = 0.0;

    for (double v : x)
        s += v * v;

    return std::sqrt(s);
}

// --------------------------------------------------
// Residual: r = b - A x
// --------------------------------------------------
inline void residual(const LinearSystem& sys,
                     const std::vector<double>& x,
                     std::vector<double>& r)
{
    const int N = sys.size();

    if ((int)x.size() != N || (int)r.size() != N)
        throw std::runtime_error("residual: dimension mismatch");

    const auto& b = sys.rhs();

    for (int i = 0; i < N; ++i)
    {
        double Ax = sys.diag(i) * x[i];

        for (const auto& [j, aij] : sys.row(i))
            Ax += aij * x[j];

        r[i] = b[i] - Ax;
    }
}

} // namespace LA
