#pragma once

#include "mesh/MeshBase.hpp"
#include <vector>

class Mesh1D : public MeshBase
{
public:
    Mesh1D(int n, double L);

    // -----------------------------
    // core
    // -----------------------------
    int size() const override;
    int dim() const override;

    MeshPoint point(int p) const override;

    // -----------------------------
    // connectivity
    // -----------------------------
    const std::vector<int>& neighbors(int p) const override;

    int neighbor(int p, NeighborDir dir) const override;
    bool hasNeighbor(int p, NeighborDir dir) const override;
    int numNeighbors(int p) const override;

    // -----------------------------
    // geometry
    // -----------------------------
    double volume(int p) const override;

    double faceArea(int p, NeighborDir dir) const override;

    double distance(int p, int q) const override;

    // ❌ REMOVE THIS (see note below)
    // double edgeArea(int p, int q) const override;

    // -----------------------------
    // boundary (FACE-based)
    // -----------------------------
    std::vector<BoundaryFace> boundaryFaces() const override;

    BoundaryContext boundaryContext(BoundaryFace face) const override;

private:
    int n_;
    double L_;
    double dx_;

    std::vector<double> x_;
    std::vector<std::vector<int>> nbrs_;
};
