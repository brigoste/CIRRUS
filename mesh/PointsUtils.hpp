#pragma once
#include "mesh/Point.hpp"

inline double& x(Point& p, std::size_t i) { return p.x[i]; }
inline double  x(const Point& p, std::size_t i) { return p.x[i]; }
