#pragma once
#include <string>
#include <vector>
#include <functional>
#include "mesh/Point.hpp"

struct BoundaryPatch
{
    std::string name;

    // indices of faces belonging to this patch
    std::vector<std::size_t> faces;

    enum class Type
    {
        Dirichlet,
        Neumann,
        Convective,
        Mixed
    } type;

    // spatially varying value support
    std::function<double(const Point&)> value;
    std::function<double(const Point&)> flux;
    std::function<double(const Point&)> h;
    std::function<double(const Point&)> Tinf;
};
