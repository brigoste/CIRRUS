#pragma once

#include <vector>
#include "bc/BoundaryFace.hpp"
#include "bc/BoundaryContext.hpp"
#include "mesh/NeighborDir.hpp"

struct MeshPoint
{
    std::vector<double> x;
};

class MeshBase
{
public:
    virtual ~MeshBase() = default;

    // -----------------------------
    // topology
    // -----------------------------
    virtual int size() const = 0;
    virtual int dim() const = 0;

    // -----------------------------
    // geometry
    // -----------------------------
    virtual MeshPoint point(int p) const = 0;
    virtual double volume(int p) const = 0;

    // -----------------------------
    // interior connectivity
    // -----------------------------
    virtual const std::vector<int>& neighbors(int p) const = 0;
    virtual int neighbor(int p, NeighborDir dir) const = 0;
    virtual bool hasNeighbor(int p, NeighborDir dir) const = 0;
    virtual int numNeighbors(int p) const = 0;

    virtual double distance(int p, int q) const = 0;
    virtual double faceArea(int p, NeighborDir dir) const = 0;
    virtual double edgeArea(int p, int q) const = 0;

    // -----------------------------
    // boundary (FACE-BASED ONLY)
    // -----------------------------

    // list of physical boundary faces in the mesh
    virtual std::vector<BoundaryFace> boundaryFaces() const = 0;

    // full geometric + topological info for a boundary face
    virtual BoundaryContext boundaryContext(BoundaryFace face) const = 0;

    // optional helper: map face → owner cell(s)
    virtual std::vector<int> faceNodes(BoundaryFace face) const = 0;

    // -----------------------------
    // future hook
    // -----------------------------
    virtual void buildConnectivity() {}
};
