#pragma once
#include <cstddef>
#include <cmath>

class MeshBase;
struct Face;

class MeshGeometry
{
public:
    // interior distance between cells
    static double cellDistance(const MeshBase& mesh,
                               std::size_t P,
                               std::size_t N);

    // boundary distance (face → cell center)
    static double faceDistance(const MeshBase& mesh,
                               const Face& face,
                               std::size_t P);
};
