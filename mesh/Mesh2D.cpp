#include "mesh/Mesh2D.hpp"
#include <stdexcept>
#include <cmath>
#include <cassert>

// -----------------------------
// Constructor
// -----------------------------
Mesh2D::Mesh2D(int nx, int ny, double Lx, double Ly)
    : nx_(nx),
      ny_(ny),
      Lx_(Lx),
      Ly_(Ly),
      dx_(Lx / (nx - 1)),
      dy_(Ly / (ny - 1)),
      x_(nx),
      y_(ny),
      nbrs_(nx * ny)
{
    for (int i = 0; i < nx_; ++i)
        x_[i] = i * dx_;

    for (int j = 0; j < ny_; ++j)
        y_[j] = j * dy_;

    // -----------------------------
    // Build adjacency graph
    // -----------------------------
    for (int j = 0; j < ny_; ++j)
    {
        for (int i = 0; i < nx_; ++i)
        {
            int p = idx(i, j);

            if (i > 0)     nbrs_[p].push_back(idx(i - 1, j));
            if (i < nx_ - 1) nbrs_[p].push_back(idx(i + 1, j));
            if (j > 0)     nbrs_[p].push_back(idx(i, j - 1));
            if (j < ny_ - 1) nbrs_[p].push_back(idx(i, j + 1));
        }
    }
}

// -----------------------------
// Core
// -----------------------------
int Mesh2D::size() const
{
    return nx_ * ny_;
}

int Mesh2D::dim() const
{
    return 2;
}

// -----------------------------
// Helpers
// -----------------------------
int Mesh2D::idx(int i, int j) const
{
    return j * nx_ + i;
}

void Mesh2D::ij(int p, int& i, int& j) const
{
    i = p % nx_;
    j = p / nx_;
}

// -----------------------------
// Geometry
// -----------------------------
MeshPoint Mesh2D::point(int p) const
{
    int i, j;
    ij(p, i, j);

    return MeshPoint{{x_[i], y_[j]}};
}

// -----------------------------
// Connectivity
// -----------------------------
const std::vector<int>& Mesh2D::neighbors(int p) const
{
    return nbrs_[p];
}

int Mesh2D::neighbor(int p, NeighborDir dir) const
{
    int i, j;
    ij(p, i, j);

    switch (dir)
    {
        case NeighborDir::W: return (i > 0) ? idx(i - 1, j) : p;
        case NeighborDir::E: return (i < nx_ - 1) ? idx(i + 1, j) : p;
        case NeighborDir::S: return (j > 0) ? idx(i, j - 1) : p;
        case NeighborDir::N: return (j < ny_ - 1) ? idx(i, j + 1) : p;

        case NeighborDir::T:
        case NeighborDir::B:
            throw std::runtime_error("Invalid NeighborDir for Mesh2D (T/B not supported)");
    }

    throw std::runtime_error("Unknown NeighborDir");
}

bool Mesh2D::hasNeighbor(int p, NeighborDir dir) const
{
    int i, j;
    ij(p, i, j);

    switch (dir)
    {
        case NeighborDir::W: return i > 0;
        case NeighborDir::E: return i < nx_ - 1;
        case NeighborDir::S: return j > 0;
        case NeighborDir::N: return j < ny_ - 1;

        case NeighborDir::T:
        case NeighborDir::B:
            throw std::runtime_error("Invalid NeighborDir for Mesh2D (T/B not supported)");
    }

    return false;
}

int Mesh2D::numNeighbors(int) const
{
    return 4;
}

// -----------------------------
// Geometry metrics
// -----------------------------
double Mesh2D::volume(int) const
{
    return dx_ * dy_;
}

double Mesh2D::faceArea(int, NeighborDir dir) const
{
    // orthogonal structured grid
    if (dir == NeighborDir::W || dir == NeighborDir::E)
        return dy_;

    if (dir == NeighborDir::S || dir == NeighborDir::N)
        return dx_;

    return 0.0;
}

double Mesh2D::distance(int p, int q) const
{
    int i1, j1, i2, j2;
    ij(p, i1, j1);
    ij(q, i2, j2);

    double dx = (i1 - i2) * dx_;
    double dy = (j1 - j2) * dy_;

    return std::sqrt(dx*dx + dy*dy);
}

// -----------------------------
// Boundary
// -----------------------------
BoundaryFace Mesh2D::faceType(int p) const
{
    int i, j;
    ij(p, i, j);

    if (i == 0) return BoundaryFace::Left;
    if (i == nx_ - 1) return BoundaryFace::Right;
    if (j == 0) return BoundaryFace::Bottom;
    if (j == ny_ - 1) return BoundaryFace::Top;

    return BoundaryFace::Interior;
}

BoundaryContext Mesh2D::boundaryContext(int p, BoundaryFace face) const
{
    BoundaryContext ctx;

    ctx.owner = p;

    int i, j;
    ij(p, i, j);

    ctx.area = 0.0;
    ctx.distance = 0.0;

    switch (face)
    {
        case BoundaryFace::Left:   // W
            ctx.area = dy_;
            ctx.distance = dx_;
            ctx.normalDir = NeighborDir::W;
            break;

        case BoundaryFace::Right:  // E
            ctx.area = dy_;
            ctx.distance = dx_;
            ctx.normalDir = NeighborDir::E;
            break;

        case BoundaryFace::Bottom: // S
            ctx.area = dx_;
            ctx.distance = dy_;
            ctx.normalDir = NeighborDir::S;
            break;

        case BoundaryFace::Top:    // N
            ctx.area = dx_;
            ctx.distance = dy_;
            ctx.normalDir = NeighborDir::N;
            break;

        default:
            assert(false && "Invalid boundary face");
    }

    return ctx;
}

std::vector<int> Mesh2D::boundaryNodes(BoundaryFace face) const
{
    std::vector<int> nodes;

    for (int p = 0; p < size(); ++p)
    {
        int i, j;
        ij(p, i, j);

        if (face == BoundaryFace::Left   && i == 0) nodes.push_back(p);
        if (face == BoundaryFace::Right  && i == nx_ - 1) nodes.push_back(p);
        if (face == BoundaryFace::Bottom && j == 0) nodes.push_back(p);
        if (face == BoundaryFace::Top    && j == ny_ - 1) nodes.push_back(p);
    }

    return nodes;
}

double Mesh2D::edgeArea(int p, int q) const
{
    int i1, j1, i2, j2;
    ij(p, i1, j1);
    ij(q, i2, j2);

    if (i1 != i2)
        return dy_; // vertical face
    else
        return dx_; // horizontal face
}
