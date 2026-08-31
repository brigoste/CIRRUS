#include "mesh/Mesh2D.hpp"
#include "mesh/MeshTypes.hpp"

#include <stdexcept>
#include <cmath>

// =========================================================
// Constructor
// =========================================================

Mesh2D::Mesh2D(int Nx, int Ny, double Lx, double Ly)
    : Nx_(Nx), Ny_(Ny), Lx_(Lx), Ly_(Ly)
{
    if (Nx_ <= 0 || Ny_ <= 0)
        throw std::runtime_error("Mesh2D: invalid resolution");

    dx_ = Lx_ / static_cast<double>(Nx_);
    dy_ = Ly_ / static_cast<double>(Ny_);

    const int Ncells = Nx_ * Ny_;
    const int Nverts = (Nx_ + 1) * (Ny_ + 1);

    centers_.resize(Ncells);
    nodes_.resize(Nverts);

    // -------------------------
    // Nodes
    // -------------------------
    for (int j = 0; j <= Ny_; ++j)
    {
        for (int i = 0; i <= Nx_; ++i)
        {
            std::size_t id = i + (Nx_ + 1) * j;

            nodes_[id].x[0] = i * dx_;
            nodes_[id].x[1] = j * dy_;
            nodes_[id].x[2] = 0.0;
        }
    }

    // -------------------------
    // Cell centers
    // -------------------------
    for (int j = 0; j < Ny_; ++j)
    {
        for (int i = 0; i < Nx_; ++i)
        {
            std::size_t c = i + Nx_ * j;

            centers_[c].x[0] = (i + 0.5) * dx_;
            centers_[c].x[1] = (j + 0.5) * dy_;
            centers_[c].x[2] = 0.0;
        }
    }

    // -------------------------
    // Faces
    // -------------------------
    const int nfaces = (Nx_ + 1) * Ny_ + Nx_ * (Ny_ + 1);
    faces_.resize(nfaces);

    std::size_t f = 0;

    // Vertical faces
    for (int j = 0; j < Ny_; ++j)
    {
        for (int i = 0; i <= Nx_; ++i)
        {
            Face& face = faces_[f];

            face.center.x[0] = i * dx_;
            face.center.x[1] = (j + 0.5) * dy_;
            face.center.x[2] = 0.0;

            face.area = dy_;
            face.normal.x = (i == 0) ? -1.0 : 1.0;
            face.normal.y = 0.0;
            face.normal.z = 0.0;

            if (i == 0)
            {
                face.owner = j * Nx_;
                face.neighbor = Face::INVALID;
                face.d = 0.5 * dx_;
            }
            else if (i == Nx_)
            {
                face.owner = (j + 1) * Nx_ - 1;
                face.neighbor = Face::INVALID;
                face.d = 0.5 * dx_;
            }
            else
            {
                int cL = (i - 1) + j * Nx_;
                int cR = i + j * Nx_;

                face.owner = cL;
                face.neighbor = cR;
                face.d = dx_;
            }

            ++f;
        }
    }

    // Horizontal faces
    for (int j = 0; j <= Ny_; ++j)
    {
        for (int i = 0; i < Nx_; ++i)
        {
            Face& face = faces_[f];

            face.center.x[0] = (i + 0.5) * dx_;
            face.center.x[1] = j * dy_;
            face.center.x[2] = 0.0;

            face.area = dx_;
            face.normal.x = 0.0;
            face.normal.y = (j == 0) ? -1.0 : 1.0;
            face.normal.z = 0.0;

            if (j == 0)
            {
                face.owner = i;
                face.neighbor = Face::INVALID;
                face.d = 0.5 * dy_;
            }
            else if (j == Ny_)
            {
                face.owner = i + (Ny_ - 1) * Nx_;
                face.neighbor = Face::INVALID;
                face.d = 0.5 * dy_;
            }
            else
            {
                int cB = i + (j - 1) * Nx_;
                int cT = i + j * Nx_;

                face.owner = cB;
                face.neighbor = cT;
                face.d = dy_;
            }

            ++f;
        }
    }
}

// =========================================================
// MeshBase interface
// =========================================================

std::size_t Mesh2D::ncells() const { return Nx_ * Ny_; }

std::size_t Mesh2D::nfaces() const { return faces_.size(); }

std::size_t Mesh2D::nnodes() const { return nodes_.size(); }

const Point& Mesh2D::node(std::size_t i) const { return nodes_.at(i); }

const Point& Mesh2D::cellCenter(std::size_t i) const { return centers_.at(i); }

const Face& Mesh2D::face(std::size_t i) const { return faces_.at(i); }

// iterators
std::vector<Face>::const_iterator Mesh2D::facesBegin() const { return faces_.begin(); }

std::vector<Face>::const_iterator Mesh2D::facesEnd() const { return faces_.end(); }

// =========================================================
// VTK INTERFACE
// =========================================================
int Mesh2D::vtkCellType(std::size_t) const
{
    return 9; // VTK_QUAD
}
void Mesh2D::cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const
{
    int i = c % Nx_;
    int j = c / Nx_;

    nodes = { static_cast<std::size_t>(i + j * (Nx_ + 1)),
              static_cast<std::size_t>((i + 1) + j * (Nx_ + 1)),
              static_cast<std::size_t>((i + 1) + (j + 1) * (Nx_ + 1)),
              static_cast<std::size_t>(i + (j + 1) * (Nx_ + 1)) };
}
