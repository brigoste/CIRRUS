#pragma once

#include "equation_systems/LinearEquationSystem.hpp"
#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Vector.hpp"
#include <vector>
#include <cmath>
#include <stdexcept>

namespace LA
{

// --------------------------------------------------
// Matrix-vector product: y = A x
// --------------------------------------------------
inline void matvec(const LinearEquationSystem& sys, const std::vector<double>& x, std::vector<double>& y)
{
    const std::size_t N = sys.size();

    if (x.size() != N || y.size() != N) 
    { 
        throw std::runtime_error("matvec: dimension mismatch"); 
    }

    for (std::size_t i = 0; i < N; ++i)
    {
        double sum = 0.0;

        // Iterate over all non-zeros in row i
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
    { 
        throw std::runtime_error("dot: size mismatch"); 
    }

    double s = 0.0;

    for (size_t i = 0; i < a.size(); ++i) 
    { 
        s += a[i] * b[i]; 
    }

    return s;
}

inline double dot(const Point& a, const Point& b)
{
    return a.x[0] * b.x[0]
         + a.x[1] * b.x[1]
         + a.x[2] * b.x[2];
}

inline double dot(const Point& a, const Vector& b)
{
    return
        a.x[0]*b.x +
        a.x[1]*b.y +
        a.x[2]*b.z;
}

inline double dot(const Vector& a, const Vector& b)
{
    return 
        a.x*b.x +
        a.y*b.y +
        a.z*b.z;
}

// --------------------------------------------------
// y += alpha * x
// --------------------------------------------------
inline void axpy(double alpha, const std::vector<double>& x, std::vector<double>& y)
{
    if (x.size() != y.size()) 
    { 
        throw std::runtime_error("axpy: size mismatch"); 
    }

    for (size_t i = 0; i < x.size(); ++i) 
    { 
        y[i] += alpha * x[i]; 
    }
}

// --------------------------------------------------
// L2 norm
// --------------------------------------------------
inline double norm2(const std::vector<double>& x)
{
    double s = 0.0;

    for (double v : x) 
    { 
        s += v * v; 
    }

    return std::sqrt(s);
}

// --------------------------------------------------
// Residual: r = b - A x
// --------------------------------------------------
inline void residual(const LinearEquationSystem& sys, const std::vector<double>& x, std::vector<double>& r)
{
    const std::size_t N = sys.size();

    if (x.size() != N || r.size() != N) 
    { 
        throw std::runtime_error("residual: dimension mismatch"); 
    }

    for (std::size_t i = 0; i < N; ++i)
    {
        double Ax = 0.0;

        for (const auto& [j, aij] : sys.row(i)) 
        { 
            Ax += aij * x[j]; 
        }

        r[i] = sys.rhs(i) - Ax;
    }
}

} // namespace LA