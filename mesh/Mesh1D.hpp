#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/Face.hpp"
#include <vector>

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

    // connectivity (canonical FV + VTK interface)
    void cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const override;
    int vtkCellType(std::size_t c) const override;

    // faces
    const Face& face(std::size_t f) const override;
    std::vector<Face>::const_iterator facesBegin() const override;
    std::vector<Face>::const_iterator facesEnd() const override;

    // queries
    std::size_t leftBoundaryFace() const;
    std::size_t rightBoundaryFace() const;

private:
    std::size_t N_;
    double L_, dx_;

    std::vector<Point> nodes_;
    std::vector<Point> centers_;
    std::vector<Face> faces_;
};
