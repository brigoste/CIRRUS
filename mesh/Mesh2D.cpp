#include "mesh/Mesh2D.hpp"
#include "bc/BCType.hpp"
#include <stdexcept>

// =========================================================
// Constructor
// =========================================================
Mesh2D::Mesh2D(int Nx, int Ny, double Lx, double Ly)
    : Nx_(Nx), Ny_(Ny), Lx_(Lx), Ly_(Ly)
{
    if (Nx_ <= 0 || Ny_ <= 0)
        throw std::runtime_error("Mesh2D: invalid grid size");

    dx_ = Lx_ / Nx_;
    dy_ = Ly_ / Ny_;

    // ---------------------------------------------------------
    // Cell centers
    // ---------------------------------------------------------
    centers_.resize(Nx_ * Ny_);

    for (int j = 0; j < Ny_; ++j)
    {
        for (int i = 0; i < Nx_; ++i)
        {
            std::size_t c = idx(i, j);

            centers_[c].x[0] = (i + 0.5) * dx_;
            centers_[c].x[1] = (j + 0.5) * dy_;
            centers_[c].x[2] = 0.0;
        }
    }

    // ---------------------------------------------------------
    // Faces (ONLY source of connectivity)
    // ---------------------------------------------------------
    faces_.clear();
    faces_.reserve((Nx_ + 1) * Ny_ + (Ny_ + 1) * Nx_);

    auto cell = [&](int i, int j) -> std::size_t
    {
        return static_cast<std::size_t>(j * Nx_ + i);
    };

    // =========================================================
    // VERTICAL FACES (x-normal)
    // =========================================================
    for (int j = 0; j < Ny_; ++j)
    {
        for (int i = 0; i <= Nx_; ++i)
        {
            Face f{};

            f.area = dy_;
            f.centroidDistance = (i == 0 || i == Nx_) ? 0.5 * dx_ : dx_;

            f.center.x[0] = i * dx_;
            f.center.x[1] = (j + 0.5) * dy_;
            f.center.x[2] = 0.0;

            f.normal = {1.0, 0.0, 0.0};

            f.isBoundary = false;

            if (i == 0)
            {
                f.isBoundary = true;
                f.owner = cell(0, j);
                f.neighbor = INVALID;
                f.normal.x[0] = -1.0;

                f.bcType = BCType::Dirichlet;
                f.value = 300.0;                // Assuming this is the set condition
            }
            else if (i == Nx_)
            {
                f.isBoundary = true;
                f.owner = cell(Nx_ - 1, j);
                f.neighbor = INVALID;

                f.bcType = BCType::Dirichlet;
                f.value = 100.0;                // Assuming this is the set condition
            }
            else
            {
                f.owner = cell(i - 1, j);
                f.neighbor = cell(i, j);
            }

            faces_.push_back(f);
        }
    }

    // =========================================================
    // HORIZONTAL FACES (y-normal)
    // =========================================================
    for (int i = 0; i < Nx_; ++i)
    {
        for (int j = 0; j <= Ny_; ++j)
        {
            Face f{};

            f.area = dx_;
            f.centroidDistance = (j == 0 || j == Ny_) ? 0.5 * dy_ : dy_;

            f.center.x[0] = (i + 0.5) * dx_;
            f.center.x[1] = j * dy_;
            f.center.x[2] = 0.0;

            f.normal = {0.0, 1.0, 0.0};

            f.isBoundary = false;

            if (j == 0)
            {
                f.isBoundary = true;
                f.owner = cell(i, 0);
                f.neighbor = INVALID;
                f.normal.x[1] = -1.0;

                f.bcType = BCType::Neumann;
                f.flux = 0.0;                // Assuming this is the set condition

                // CONVECTIVE
                // f.bcType = BCType::Convective;
                // f.h = h;
                // f.Tinf = Tinf;                // Assuming this is the set condition
            }
            else if (j == Ny_)
            {
                f.isBoundary = true;
                f.owner = cell(i, Ny_ - 1);
                f.neighbor = INVALID;

                f.bcType = BCType::Dirichlet;
                f.value = 300.0;                // Assuming this is the set condition
            }
            else
            {
                f.owner = cell(i, j - 1);
                f.neighbor = cell(i, j);
            }

            faces_.push_back(f);
        }
    }
}

// =========================================================
// Basic mesh queries
// =========================================================

std::size_t Mesh2D::ncells() const
{
    return centers_.size();
}

std::size_t Mesh2D::nnodes() const
{
    return (Nx_ + 1) * (Ny_ + 1);
}

Point Mesh2D::cellCenter(std::size_t i) const
{
    return centers_.at(i);
}

Point Mesh2D::node(std::size_t i) const
{
    int nxN = Nx_ + 1;
    int j = i / nxN;
    int i0 = i % nxN;

    return Point{{i0 * dx_, j * dy_, 0.0}};
}

std::size_t Mesh2D::cellNodeCount(std::size_t) const
{
    return 4;
}

std::size_t Mesh2D::cellNode(std::size_t c, std::size_t k) const
{
    int i = c % Nx_;
    int j = c / Nx_;

    int nxN = Nx_ + 1;

    switch (k)
    {
        case 0: return j * nxN + i;
        case 1: return j * nxN + i + 1;
        case 2: return (j + 1) * nxN + i + 1;
        case 3: return (j + 1) * nxN + i;
        default:
            throw std::runtime_error("Invalid cellNode index");
    }
}

double Mesh2D::cellVolume(std::size_t) const
{
    return dx_ * dy_;
}

// =========================================================
// Face access
// =========================================================

std::size_t Mesh2D::nFaces() const
{
    return faces_.size();
}

const Face& Mesh2D::face(std::size_t f) const
{
    return faces_.at(f);
}

// =========================================================
// Helpers
// =========================================================

void Mesh2D::ij(int id, int& i, int& j) const
{
    i = id % Nx_;
    j = id / Nx_;
}

// Optional BC indexing helper (kept if you still use it elsewhere)
std::vector<std::size_t>
Mesh2D::boundaryFaceIndices(BoundaryFace f) const
{
    std::vector<std::size_t> result;

    if (f == BoundaryFace::Left)
    {
        for (int j = 0; j < Ny_; ++j)
            result.push_back(j * (Nx_ + 1));
    }
    else if (f == BoundaryFace::Right)
    {
        for (int j = 0; j < Ny_; ++j)
            result.push_back(j * (Nx_ + 1) + Nx_);
    }
    else if (f == BoundaryFace::Bottom)
    {
        int offset = (Nx_ + 1) * Ny_;
        for (int i = 0; i < Nx_; ++i)
            result.push_back(offset + i);
    }
    else if (f == BoundaryFace::Top)
    {
        int offset = (Nx_ + 1) * Ny_ + Nx_ * (Ny_ + 1);
        for (int i = 0; i < Nx_; ++i)
            result.push_back(offset + i);
    }
    else
    {
        throw std::runtime_error("Invalid BoundaryFace");
    }

    return result;
}
