#include "mesh/Mesh1D.hpp"
#include "bc/BCType.hpp"
#include <stdexcept>

// =========================================================
// Constructor
// =========================================================
Mesh1D::Mesh1D(std::size_t N, double L)
    : N_(N), L_(L)
{
    if (N_ == 0)
        throw std::runtime_error("Mesh1D: N must be > 0");

    dx_ = L_ / static_cast<double>(N_);

    // ---------------------------------------------------------
    // Cell centers
    // ---------------------------------------------------------
    centers_.resize(N_);

    for (std::size_t i = 0; i < N_; ++i)
    {
        centers_[i].x[0] = (i + 0.5) * dx_;
        centers_[i].x[1] = 0.0;
        centers_[i].x[2] = 0.0;
    }

    // ---------------------------------------------------------
    // Build faces (N + 1 faces)
    // ---------------------------------------------------------
    faces_.resize(N_ + 1);

    for (std::size_t f = 0; f <= N_; ++f)
    {
        Face& face = faces_[f];

        face.center.x[0] = f * dx_;
        face.center.x[1] = 0.0;
        face.center.x[2] = 0.0;

        face.area = 1.0;

        face.normal.x[0] = 1.0;
        face.normal.x[1] = 0.0;
        face.normal.x[2] = 0.0;

        // -----------------------------------------------------
        // Boundary faces
        // -----------------------------------------------------
        if (f == 0)
        {
            face.isBoundary = true;
            face.owner = 0;
            face.neighbor = INVALID;

            face.normal.x[0] = -1.0;
            face.centroidDistance = 0.5 * dx_;

            // default BC (can be overwritten by applyBoundaryConfig)
            face.bcType = BCType::Dirichlet;
            face.value = 0.0;
        }
        else if (f == N_)
        {
            face.isBoundary = true;
            face.owner = N_ - 1;
            face.neighbor = INVALID;

            face.normal.x[0] = 1.0;
            face.centroidDistance = 0.5 * dx_;

            face.bcType = BCType::Dirichlet;
            face.value = 0.0;
        }
        else
        {
            // -------------------------------------------------
            // Interior face
            // -------------------------------------------------
            face.isBoundary = false;
            face.owner = f - 1;
            face.neighbor = f;

            face.centroidDistance = dx_;
        }
    }
}

// =========================================================
// Core API
// =========================================================

std::size_t Mesh1D::ncells() const
{
    return N_;
}

std::size_t Mesh1D::nnodes() const
{
    return N_ + 1;
}

Point Mesh1D::cellCenter(std::size_t i) const
{
    return centers_.at(i);
}

Point Mesh1D::node(std::size_t i) const
{
    Point p;
    p.x[0] = i * dx_;
    p.x[1] = 0.0;
    p.x[2] = 0.0;
    return p;
}

std::size_t Mesh1D::cellNodeCount(std::size_t) const
{
    return 2;
}

std::size_t Mesh1D::cellNode(std::size_t c, std::size_t k) const
{
    if (k == 0) return c;
    if (k == 1) return c + 1;
    throw std::runtime_error("Mesh1D::cellNode invalid index");
}

double Mesh1D::cellVolume(std::size_t) const
{
    return dx_;
}

// =========================================================
// Face API
// =========================================================

std::size_t Mesh1D::nFaces() const
{
    return faces_.size();
}

const Face& Mesh1D::face(std::size_t f) const
{
    return faces_.at(f);
}

// =========================================================
// Dimension
// =========================================================

std::size_t Mesh1D::dim() const
{
    return 1;
}

// =========================================================
// Apply boundary conditions (ONLY valid BC entry point)
// =========================================================
void Mesh1D::applyBoundaryConfig(const std::vector<BoundaryConfig>& bcs)
{
    for (const auto& bc : bcs)
    {
        Face* f = nullptr;

        if (bc.face == BoundaryFace::Left)
        {
            f = &faces_.front();
        }
        else if (bc.face == BoundaryFace::Right)
        {
            f = &faces_.back();
        }
        else
        {
            throw std::runtime_error("Invalid boundary face for Mesh1D");
        }

        f->isBoundary = true;
        f->bcType = bc.type;

        f->value = bc.value;
        f->flux  = bc.flux;
        f->h     = bc.h;
        f->Tinf  = bc.Tinf;
    }
}
std::vector<Face>::const_iterator Mesh1D::facesBegin() const
{
    return faces_.begin();
}
std::vector<Face>::const_iterator Mesh1D::facesEnd() const
{
    return faces_.end();
}
