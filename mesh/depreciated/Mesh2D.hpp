#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Cell.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Point.hpp"

class Mesh2D : public MeshBase
{
public:
    Mesh2D(std::size_t nx, std::size_t ny,
               double lx, double ly);

    std::size_t ncells() const override { return cells_.size(); }
    std::size_t nfaces() const override { return faces_.size(); }

    const Cell& cell(std::size_t i) const override { return cells_[i]; }
    const Face& face(std::size_t i) const override { return faces_[i]; }

    const Point& cellCenter(std::size_t i) const override { return cells_[i].center; }

    const double cellVolume(std::size_t c) const override { return cellVolume(c); }

    void cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const override;

    int vtkCellType(std::size_t c) const override;

    std::vector<Face>::const_iterator facesBegin() const override { return faces_.begin(); }
    std::vector<Face>::const_iterator facesEnd() const override { return faces_.end(); }

private:
    std::size_t Nx_, Ny_, Lx_, Ly_;
    double dx_, dy_;

    std::vector<Cell> cells_;
    std::vector<Face> faces_;

    std::size_t idx(std::size_t i, std::size_t j) const;

    void buildCells();
    void buildFaces();
};
