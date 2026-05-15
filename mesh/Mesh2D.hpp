#pragma once

#include "mesh/MeshBase.hpp"
#include "config/BoundaryConfig.hpp"
#include "mesh/Face.hpp"
#include <vector>
#include <optional>
#include <stdexcept>

class Mesh2D : public MeshBase
{
public:
    Mesh2D(int Nx, int Ny, double Lx, double Ly);

    std::size_t ncells() const override;
    std::size_t nnodes() const override;

    Point cellCenter(std::size_t i) const override;
    Point node(std::size_t i) const override;

    std::size_t cellNodeCount(std::size_t i) const override;
    std::size_t cellNode(std::size_t icell, std::size_t k) const override;

    double cellVolume(std::size_t i) const override;

    int leftNeighbor(std::size_t i) const;
    int rightNeighbor(std::size_t i) const;

    std::size_t dim() const override { return 2; }

    // =========================================================
    // FACE SYSTEM
    // =========================================================
    std::size_t nFaces() const override;
    const Face& face(std::size_t f) const override;

private:
    std::vector<std::size_t> Mesh2D::boundaryFaceIndices(BoundaryFace f) const;
    const std::vector<std::size_t>& cellFaces(std::size_t c) const;
    virtual std::vector<Face>::const_iterator Mesh2D::facesBegin() const override;
    virtual std::vector<Face>::const_iterator Mesh2D::facesEnd() const override;

    int Nx_, Ny_;
    double Lx_, Ly_;
    double dx_, dy_;

    std::vector<Point> centers_;
    std::vector<Face> faces_;
    std::vector<std::vector<std::size_t>> cellFaces_;

    int idx(int i, int j) const { return i + Nx_ * j; }
    void ij(int id, int& i, int& j) const;
};
