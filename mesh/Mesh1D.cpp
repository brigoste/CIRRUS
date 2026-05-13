#include "mesh/Mesh1D.hpp"
#include <stdexcept>
#include <cmath>

// -----------------------------
// Constructor
// -----------------------------
Mesh1D::Mesh1D(int n, double L)
    : n_(n),
      L_(L),
      dx_(L / (n - 1)),
      x_(n),
      nbrs_(n)
{
    for (int i = 0; i < n_; ++i)
    {
        x_[i] = i * dx_;

        if (i > 0)
            nbrs_[i].push_back(i - 1);

        if (i < n_ - 1)
            nbrs_[i].push_back(i + 1);
    }
}

// -----------------------------
// Core
// -----------------------------
int Mesh1D::size() const { return n_; }

int Mesh1D::dim() const { return 1; }

// -----------------------------
// Geometry
// -----------------------------
MeshPoint Mesh1D::point(int p) const
{
    return MeshPoint{{x_[p]}};
}

// -----------------------------
// Connectivity
// -----------------------------
const std::vector<int>& Mesh1D::neighbors(int p) const
{
    return nbrs_[p];
}

int Mesh1D::neighbor(int p, NeighborDir dir) const
{
    if (dir == NeighborDir::W)
        return (p > 0) ? p - 1 : p;

    if (dir == NeighborDir::E)
        return (p < n_ - 1) ? p + 1 : p;

    throw std::runtime_error("Invalid NeighborDir in Mesh1D");
}

bool Mesh1D::hasNeighbor(int p, NeighborDir dir) const
{
    if (dir == NeighborDir::W) return p > 0;
    if (dir == NeighborDir::E) return p < n_ - 1;
    return false;
}

int Mesh1D::numNeighbors(int) const
{
    return 2;
}

// -----------------------------
// Geometry metrics
// -----------------------------
double Mesh1D::volume(int) const
{
    return dx_;
}

double Mesh1D::faceArea(int, NeighborDir) const
{
    // 1D convention: face area = 1
    return 1.0;
}

double Mesh1D::distance(int p, int q) const
{
    return std::abs(x_[p] - x_[q]);
}

// =====================================================
// BOUNDARY (ONLY IMPLEMENTED INTERFACE)
// =====================================================

std::vector<BoundaryFace> Mesh1D::boundaryFaces() const
{
    return {BoundaryFace::Left, BoundaryFace::Right};
}

BoundaryContext Mesh1D::boundaryContext(int owner, BoundaryFace face) const
{
    BoundaryContext ctx{};

    ctx.owner = owner;
    ctx.neighbor = -1;

    ctx.area = 1.0;
    ctx.distance = dx_ / 2.0;

    if (face == BoundaryFace::Left)
        ctx.normalDir = NeighborDir::W;
    else if (face == BoundaryFace::Right)
        ctx.normalDir = NeighborDir::E;
    else
        throw std::runtime_error("Invalid BoundaryFace");

    return ctx;
}

double Mesh1D::edgeArea(int, int) const
{
    return 1.0;
}
std::vector<int> Mesh1D::faceNodes(BoundaryFace face) const
{
    if (face == BoundaryFace::Left)
        return {0};

    if (face == BoundaryFace::Right)
        return {n_ - 1};

    return {};
}
int Mesh1D::faceOwner(BoundaryFace face) const
{
    if (face == BoundaryFace::Left)
        return 0;

    if (face == BoundaryFace::Right)
        return n_ - 1;

    throw std::runtime_error("Invalid BoundaryFace in Mesh1D");
}
