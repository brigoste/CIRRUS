#pragma once
#include <array>

struct Point
{
    std::array<double, 3> x{};  // fixed max dimension

    double& operator[](std::size_t i) { return x[i]; }
    double  operator[](std::size_t i) const { return x[i]; }

    std::size_t dim() const { return 3; }
};

inline Point operator+(const Point& a, const Point& b);
inline Point operator-(const Point& a, const Point& b);
inline Point operator*(double s, const Point& a);
