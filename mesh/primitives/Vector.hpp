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

    double magnitudeSquared() { return x*x + y*y + z*z; }

    double& operator[](std::size_t i) { return (&x)[i]; }

    double operator[](std::size_t i) const { return (&x)[i]; }

    Vector& operator+=(const Vector& other)
    {
        x += other.x;
        y += other.y;
        z += other.z;
        return *this;
    }

    Vector& operator-=(const Vector& other)
    {
        x -= other.x;
        y -= other.y;
        z -= other.z;
        return *this;
    }

    Vector& operator*=(double s)
    {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    Vector& operator/=(double s)
    {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
};

// Free operators

inline Vector operator+(Vector a, const Vector& b)
{
    a += b;
    return a;
}

inline Vector operator-(Vector a, const Vector& b)
{
    a -= b;
    return a;
}

inline Vector operator*(double s, Vector v)
{
    v *= s;
    return v;
}

inline Vector operator*(Vector v, double s)
{
    v *= s;
    return v;
}

inline Vector operator/(Vector v, double s)
{
    v /= s;
    return v;
}
