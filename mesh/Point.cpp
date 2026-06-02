#include "mesh/Point.hpp"

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
