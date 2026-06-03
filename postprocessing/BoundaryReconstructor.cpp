#include "postprocessing/BoundaryReconstructor.hpp"

#include <stdexcept>

Field1D BoundaryReconstructor::reconstruct(
    const MeshBase& mesh,
    const BoundaryPatchSystem& boundary,
    const PhysicsModel& model,
    const std::vector<double>& phi)
{
    Field1D out;

    const std::size_t n = mesh.ncells();
    out.x.reserve(n + 2);
    out.phi.reserve(n + 2);

    if (n == 0)
        return out;

    // LEFT BOUNDARY
    {
        const Face& f = mesh.face(0);
        const auto* bc = boundary.get(0);

        const double dx =
            MeshGeometry::faceDistance(mesh, f, 0);

        double Tw = model.reconstructBoundaryValue(
            *bc,
            phi.front(),
            dx,
            true);

        out.x.push_back(f.center[0]);
        out.phi.push_back(Tw);
    }

    // =====================================================
    // INTERIOR CELLS
    // =====================================================
    for (std::size_t c = 0; c < n; ++c)
    {
        out.x.push_back(mesh.cellCenter(c)[0]);
        out.phi.push_back(phi[c]);
    }

    // RIGHT BOUNDARY
    {
        const std::size_t fIdx = mesh.nfaces() - 1;

        const Face& f = mesh.face(fIdx);
        const auto* bc = boundary.get(fIdx);

        const double dx =
            MeshGeometry::faceDistance(mesh, f, n - 1);

        double Tw = model.reconstructBoundaryValue(
            *bc,
            phi.back(),
            dx,
            false);

        out.x.push_back(f.center[0]);
        out.phi.push_back(Tw);
    }

    return out;
}
