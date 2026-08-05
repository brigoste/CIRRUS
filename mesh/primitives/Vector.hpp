#pragma once

#include <cmath>

struct Vector
{
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    Vector() = default;

    Vector(
        double x_,
        double y_,
        double z_ = 0.0
    )
    :
    x(x_), y(y_), z(z_)
    {}

    double magnitude() const { return std::sqrt(x*x + y*y + z*z); }
    double magnitudeSquared() const { return x*x + y*y + z*z; }
};
