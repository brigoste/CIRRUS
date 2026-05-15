#pragma once

#include <cstddef>
#include <limits>
#include <vector>
#include <optional>
#include "mesh/Point.hpp"
// #include "bc/BoundaryConditionDescriptor.hpp"
// #include "bc/BoundaryContext.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/Face.hpp"

static constexpr std::size_t INVALID = std::numeric_limits<std::size_t>::max();

class MeshBase
{
public:
    virtual ~MeshBase() = default;

    // topology
    virtual std::size_t ncells() const = 0;
    virtual std::size_t nnodes() const = 0;

    // geometry
    virtual Point cellCenter(std::size_t i) const = 0;
    virtual double cellVolume(std::size_t i) const = 0;

    // nodes
    virtual Point node(std::size_t i) const = 0;
    virtual std::size_t cellNodeCount(std::size_t i) const = 0;
    virtual std::size_t cellNode(std::size_t c, std::size_t k) const = 0;

    // faces
    virtual std::size_t nFaces() const = 0;
    virtual const Face& face(std::size_t f) const = 0;
    
    virtual std::size_t dim() const = 0;

    const std::vector<std::size_t>& cellFaces(std::size_t c) const;
    // const std::vector<Face>& faces() const;

    // NEW: iterator-style access
    virtual std::vector<Face>::const_iterator facesBegin() const = 0;
    virtual std::vector<Face>::const_iterator facesEnd() const = 0;
};
