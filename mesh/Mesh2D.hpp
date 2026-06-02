#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/Face.hpp"

#include <vector>
#include <cstddef>

class Mesh2D : public MeshBase
{
public:
    Mesh2D(int Nx, int Ny, double Lx, double Ly);

    // =========================
    // MeshBase interface (REQUIRED)
    // =========================
    std::size_t dim() const override { return 2; }

    std::size_t ncells() const override;
    std::size_t nnodes() const override;
    std::size_t nfaces() const override;

    const Point& node(std::size_t i) const override;
    const Point& cellCenter(std::size_t i) const override;

    // std::size_t cellNodeCount(std::size_t c) const override;
    // std::size_t cellNode(std::size_t c, std::size_t k) const override;

    void cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const override;
    int vtkCellType(std::size_t c) const override;

    const Face& face(std::size_t f) const override;

    std::vector<Face>::const_iterator facesBegin() const override;
    std::vector<Face>::const_iterator facesEnd() const override;

private:
    int Nx_, Ny_;
    double Lx_, Ly_;
    double dx_, dy_;

    std::vector<Point> nodes_;
    std::vector<Point> centers_;
    std::vector<Face> faces_;

    std::vector<std::vector<std::size_t>> cellNodes_;

    std::size_t idx(int i, int j) const { return i + Nx_ * j; }
};
