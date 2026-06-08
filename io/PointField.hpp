#pragma once

#include <vector>
#include "mesh/primitives/Point.hpp"

struct PointField
{
    std::vector<Point> x;     // spatial location
    std::vector<double> phi;  // scalar value

    std::size_t dim = 1;   // or 2 or 3

    std::size_t size() const { return phi.size(); }

    void clear()
    {
        x.clear();
        phi.clear();
    }

    void reserve(std::size_t n)
    {
        x.reserve(n);
        phi.reserve(n);
    }

    void push_back(const Point& p, double v)
    {
        x.push_back(p);
        phi.push_back(v);
    }
};
