#pragma once

#include <cstddef>

class MeshBase;

struct UpwindStencilCells
{
    std::size_t upwind;
    std::size_t downwind;
    std::size_t upstream;
};

UpwindStencilCells findUpwindStencilCells(
    const MeshBase& mesh,
    std::size_t owner,
    std::size_t faceIndex,
    double flux
);
