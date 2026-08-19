#pragma once

#include <cstddef>
#include <vector>
#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/primitives/Cell.hpp"

class MeshBase {
public:
    virtual ~MeshBase() = default;

    // topology
    virtual std::size_t dim() const = 0;
    virtual std::size_t ncells() const = 0;
    virtual std::size_t nnodes() const = 0;
    virtual std::size_t nfaces() const = 0;

    // geometry
    virtual const Point& node(std::size_t i) const = 0;
    virtual const Point& cellCenter(std::size_t c) const = 0;
    virtual double faceDistance(std::size_t f) const = 0;
    virtual double distance(const Point& a, const Point& b) const = 0;
    virtual double getLx() const = 0;
    virtual double getLy() const = 0;
    virtual double getLz() const = 0;

    // connectivity (canonical)
    virtual void cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const = 0;

    virtual int vtkCellType(std::size_t c) const = 0;

    // faces
    virtual const Face& face(std::size_t f) const = 0;
    virtual std::vector<Face>::const_iterator facesBegin() const = 0;
    virtual std::vector<Face>::const_iterator facesEnd() const = 0;

    // virtual std::vector<std::size_t> boundaryFaces() const = 0;
    virtual const std::vector<std::size_t>& boundaryFaces(std::size_t group) const = 0;
    virtual std::size_t nBoundaryGroups() const = 0;

    // cells
    virtual const Cell& cell(std::size_t i) const = 0;
    virtual std::size_t findCell(const Point& position) const = 0;

    // for use in test case validation
    virtual double cellVolume(std::size_t c) const = 0;
    
    static constexpr std::size_t INVALID = static_cast<std::size_t>(-1);    
};
