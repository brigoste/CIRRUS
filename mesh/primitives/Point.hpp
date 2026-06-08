#pragma once
#include <array>

struct Point
{
    std::array<double, 3> x{};  // fixed max dimension

    double& operator[](std::size_t i) { return x[i]; }
    double  operator[](std::size_t i) const { return x[i]; }

    std::size_t dim() const { return 3; }
};

inline Point operator+(const Point& a, const Point& b)
{
    return Point{
        {
            a.x[0] + b.x[0],
            a.x[1] + b.x[1],
            a.x[2] + b.x[2]
        }
    };
}

inline Point operator-(const Point& a, const Point& b)
{
    return Point{
        {
            a.x[0] - b.x[0],
            a.x[1] - b.x[1],
            a.x[2] - b.x[2]
        }
    };
}

inline Point operator*(double s, const Point& a)
{
    return Point{
        {
            s * a.x[0],
            s * a.x[1],
            s * a.x[2]
        }
    };
}
