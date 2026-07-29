#include "mesh/MeshGeometry.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"

double MeshGeometry::cellDistance(const MeshBase& mesh,
                                   std::size_t P,
                                   std::size_t N)
{
    const auto& xP = mesh.cellCenter(P);
    const auto& xN = mesh.cellCenter(N);

    return std::abs(xN[0] - xP[0]);
}

double MeshGeometry::faceDistance(const MeshBase& mesh,
                                   const Face& face,
                                   std::size_t P)
{
    const auto& xP = mesh.cellCenter(P);
    const auto& xf = face.center;

    return std::abs(xf[0] - xP[0]);
}
