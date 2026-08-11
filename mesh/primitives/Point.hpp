#pragma once

#include <array>
#include "mesh/primitives/Vector.hpp"

struct Point
{
    std::array<double, 3> x{};

    double& operator[](std::size_t i) { return x[i]; }
    double operator[](std::size_t i) const { return x[i]; }

    std::size_t dim() const { return 3; }
};

// Point - Point = Vector
inline Vector operator-(const Point& a, const Point& b)
{
    return Vector{
        a.x[0] - b.x[0],
        a.x[1] - b.x[1],
        a.x[2] - b.x[2]
    };
}

// Point + Vector = Point
inline Point operator+(const Point& p, const Vector& v)
{
    return Point{
        {
            p.x[0] + v.x,
            p.x[1] + v.y,
            p.x[2] + v.z
        }
    };
}

// Point - Vector = Point
inline Point operator-(const Point& p, const Vector& v)
{
    return Point{
        {
            p.x[0] - v.x,
            p.x[1] - v.y,
            p.x[2] - v.z
        }
    };
}
