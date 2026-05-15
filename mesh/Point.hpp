#pragma once
#include <array>

struct Point
{
    std::array<double, 3> x{};  // fixed max dimension

    double& operator[](std::size_t i) { return x[i]; }
    double  operator[](std::size_t i) const { return x[i]; }

    std::size_t dim() const { return 3; }
};
