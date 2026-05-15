#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/Face.hpp"
#include "config/BoundaryConfig.hpp"

#include <vector>
#include <cstddef>

class Mesh1D : public MeshBase
{
public:
    Mesh1D(std::size_t N, double L);

    // =========================================================
    // MeshBase interface
    // =========================================================
    std::size_t ncells() const override;
    std::size_t nnodes() const override;

    Point cellCenter(std::size_t i) const override;
    Point node(std::size_t i) const override;

    std::size_t cellNodeCount(std::size_t i) const override;
    std::size_t cellNode(std::size_t icell, std::size_t k) const override;

    double cellVolume(std::size_t i) const override;

    std::size_t dim() const override;

    // =========================================================
    // Face access (core FV abstraction)
    // =========================================================
    std::size_t nFaces() const;
    const Face& face(std::size_t f) const;

    // =========================================================
    // Boundary condition binding (ONLY entry point)
    // =========================================================
    void applyBoundaryConfig(const std::vector<BoundaryConfig>& bcs);

private:
    std::size_t N_;
    double L_;
    double dx_;

    std::vector<Point> centers_;
    std::vector<Face> faces_;

    
    virtual std::vector<Face>::const_iterator Mesh1D::facesBegin() const override;
    virtual std::vector<Face>::const_iterator Mesh1D::facesEnd() const override;
};
