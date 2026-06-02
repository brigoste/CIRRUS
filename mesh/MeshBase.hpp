#pragma once
#include <cstddef>
#include <vector>
#include "mesh/Point.hpp"
#include "mesh/Face.hpp"

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

    // connectivity (canonical)
    virtual void cellNodes(
        std::size_t c,
        std::vector<std::size_t>& nodes) const = 0;

    virtual int vtkCellType(std::size_t c) const = 0;

    // faces
    virtual const Face& face(std::size_t f) const = 0;
    virtual std::vector<Face>::const_iterator facesBegin() const = 0;
    virtual std::vector<Face>::const_iterator facesEnd() const = 0;
};
