#pragma once

#include "mesh/MeshBase.hpp"
#include <vector>

class Mesh2D : public MeshBase
{
public:
    Mesh2D(int nx, int ny, double Lx, double Ly);

    // -----------------------------
    // Core
    // -----------------------------
    int size() const override;
    int dim() const override;

    MeshPoint point(int p) const override;

    // -----------------------------
    // Connectivity (GRAPH-BASED)
    // -----------------------------
    const std::vector<int>& neighbors(int p) const override;

    int neighbor(int p, NeighborDir dir) const override;
    bool hasNeighbor(int p, NeighborDir dir) const override;
    int numNeighbors(int p) const override;

    // -----------------------------
    // Geometry
    // -----------------------------
    double volume(int p) const override;
    double faceArea(int p, NeighborDir dir) const override;
    double distance(int p, int q) const override;

    // -----------------------------
    // Boundary
    // -----------------------------
    virtual BoundaryFace faceType(int p) const = 0;
    virtual BoundaryContext boundaryContext(int owner, BoundaryFace face) const override;
    virtual std::vector<int> boundaryNodes(BoundaryFace face) const = 0;
    double edgeArea(int p, int q) const override;

    // reserved hook for unstructured meshes
    void buildConnectivity() override {}

private:
    int nx_, ny_;
    double Lx_, Ly_;
    double dx_, dy_;

    std::vector<double> x_, y_;

    // graph adjacency
    std::vector<std::vector<int>> nbrs_;

    void ij(int p, int& i, int& j) const;
    int idx(int i, int j) const;
};
