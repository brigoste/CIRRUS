#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Patch.hpp"
#include <vector>
#include <array>

class Mesh1D : public MeshBase
{
public:
    Mesh1D(std::size_t N, double L);

    // topology
    std::size_t dim() const override { return 1; }
    std::size_t ncells() const override;
    std::size_t nnodes() const override;
    std::size_t nfaces() const override;

    // geometry
    const Point& node(std::size_t i) const override;
    const Point& cellCenter(std::size_t c) const override;
    double faceDistance(std::size_t f) const override;
    double distance(const Point& a, const Point& b) const override;
    double getLx() const override {return L_;}
    double getLy() const override {return 0.0;}
    double getLz() const override {return 0.0;}

    // connectivity
    void cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const override;
    int vtkCellType(std::size_t c) const override;

    // faces
    const Face& face(std::size_t f) const override;
    std::vector<Face>::const_iterator facesBegin() const override;
    std::vector<Face>::const_iterator facesEnd() const override;

    // cells
    const Cell& cell(std::size_t i) const override;

    // boundary interface (MeshBase contract)
    std::size_t nBoundaryGroups() const override;

    const std::vector<std::size_t>& boundaryFaces(std::size_t group) const override;

    // optional helper (mesh-specific convenience)
    std::size_t toGroup(Patch p) const { return static_cast<std::size_t>(p); }
    std::size_t leftBoundaryFace() const { return boundaryGroups_[toGroup(Patch::LEFT)].front(); }

    std::size_t rightBoundaryFace() const { return boundaryGroups_[toGroup(Patch::RIGHT)].front(); }

    double cellVolume(std::size_t) const override { return dx_; }

private:
    std::size_t N_;
    double L_, dx_;

    std::array<std::vector<std::size_t>, 2> boundaryGroups_;

    std::vector<Point> nodes_;
    std::vector<Point> centers_;
    std::vector<Face> faces_;
    std::vector<Cell> cells_;
};
