#pragma once

#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Cell.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Patch.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include <cmath>

class QuadMesh2D : public MeshBase
{
public:
    QuadMesh2D(std::size_t nx, std::size_t ny, double lx, double ly);

    std::size_t ncells() const override { return cells_.size(); }
    std::size_t nfaces() const override { return faces_.size(); }

    const Cell& cell(std::size_t i) const override { return cells_[i]; }
    const Face& face(std::size_t i) const override { return faces_[i]; }

    const Point& cellCenter(std::size_t i) const override { return cells_[i].center; }

    const std::vector<std::size_t>& boundaryFaces(std::size_t group) const override {return boundaryGroups_.at(group);}
    std::size_t nBoundaryGroups() const override { return boundaryGroups_.size(); }
    std::size_t toGroup(Patch p) const { return static_cast<std::size_t>(p);}

    std::size_t nnodes() const override { return nodes_.size(); }
    const Point& node(std::size_t i) const override { return nodes_.at(i); }

    double faceDistance(std::size_t f) const override
    {
        const Face& face = faces_.at(f);
        return std::abs( LA::dot(face.dPN, face.normal) );
    }

    double distance( const Point& a, const Point& b) const override;

    void cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const override;

    std::size_t dim() const override { return 2; }
    int vtkCellType(std::size_t) const override { return 9; }

    std::vector<Face>::const_iterator facesBegin() const override { return faces_.begin(); }
    std::vector<Face>::const_iterator facesEnd() const override { return faces_.end(); }

    double cellVolume(std::size_t) const override { return dx_ * dy_; }

    double getLx() const override {return Lx_;}
    double getLy() const override {return Ly_;}
    double getLz() const override {return 0.0;}

private:
    std::size_t nx_, ny_;
    double dx_, dy_;
    double Lx_, Ly_;

    std::array<std::vector<std::size_t>, 4> boundaryGroups_;

    std::vector<Cell> cells_;
    std::vector<Face> faces_;
    std::vector<Point> nodes_;

    std::size_t idx(std::size_t i, std::size_t j) const;

    void buildCells();
    void buildFaces(); 
    void buildNodes();    
};
