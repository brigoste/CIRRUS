#include "postprocessing/BoundaryReconstructor.hpp"
#include "io/PointField.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/MeshGeometry.hpp"

PointField BoundaryReconstructor::reconstruct(
    const MeshBase& mesh,
    const BoundaryPatchSystem& boundary,
    const ScalarTransportModel& model,
    const ScalarField& field)
{
    PointField out;
    out.dim = mesh.dim();

    const std::size_t nc = mesh.ncells();
    const std::size_t ng = mesh.nBoundaryGroups();

    out.reserve(nc + mesh.nfaces()); 
    // safe upper bound for reconstruction

    // =====================================================
    // 1. CELL CENTERS
    // =====================================================
    for (std::size_t c = 0; c < nc; ++c)
    {
        out.push_back(mesh.cellCenter(c), field[c]);
    }

    // =====================================================
    // 2. BOUNDARY RECONSTRUCTION
    // =====================================================
    for (std::size_t g = 0; g < ng; ++g)
    {
        const auto& faces = mesh.boundaryFaces(g);
        const auto* bc = boundary.getGroup(g);

        if (!bc) { continue; }

        for (std::size_t fIdx : faces)
        {
            const Face& f = mesh.face(fIdx);

            const std::size_t c = f.owner;

            const double dx = MeshGeometry::faceDistance(mesh, f, c);

            const bool isInward = (f.normal[0] < 0.0 || f.normal[1] < 0.0 || f.normal[2] < 0.0);

            const double phiB = model.reconstructBoundaryValue( *bc, field[c], dx, isInward );

            out.push_back(f.center, phiB);
        }
    }

    return out;
}
