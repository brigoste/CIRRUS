#include "mesh/Mesh1D.hpp"
#include "mesh/Point.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include <stdexcept>
#include <cassert>
#include <cmath>

// ===============================
// Constructor
// ===============================
Mesh1D::Mesh1D(std::size_t N, double L)
    : N_(N), L_(L)
{
    if (N_ == 0)
        throw std::runtime_error("Mesh1D: N must be > 0");

    dx_ = L_ / static_cast<double>(N_);

    nodes_.resize(N_ + 1);
    centers_.resize(N_);
    faces_.resize(N_ + 1);

    for (std::size_t i = 0; i <= N_; ++i)
        nodes_[i].x[0] = i * dx_;

    for (std::size_t i = 0; i < N_; ++i)
        centers_[i].x[0] = (i + 0.5) * dx_;

    for (std::size_t f = 0; f <= N_; ++f)
    {
        Face& face = faces_[f];

        face.center.x[0] = f * dx_;
        face.area = 1.0;

        if (f == 0)
        {
            face.owner = 0;
            face.neighbor = Face::INVALID;
            face.normal.x[0] = -1.0;

            face.dPN = 0.0;
        }
        else if (f == N_)
        {
            face.owner = N_ - 1;
            face.neighbor = Face::INVALID;
            face.normal.x[0] = 1.0;

            face.dPN = 0.0;
        }
        else
        {
            face.owner = f - 1;
            face.neighbor = f;
            face.normal.x[0] = 1.0;

            // THIS is now the ONLY place interior spacing is defined
            face.dPN = dx_;
        }
    }

    if (faces_.size() != N_ + 1)
        throw std::runtime_error("Invalid 1D mesh topology");
    
    for (std::size_t f = 1; f < N_; ++f)
    {
        assert(faces_[f].owner == f - 1);
        assert(faces_[f].neighbor == f);
    }
}

// =========================================================
// MeshBase interface
// =========================================================

std::size_t Mesh1D::ncells() const { return N_; }
std::size_t Mesh1D::nnodes() const { return nodes_.size(); }
std::size_t Mesh1D::nfaces() const { return faces_.size(); }

const Point& Mesh1D::node(std::size_t i) const { return nodes_.at(i); }
const Point& Mesh1D::cellCenter(std::size_t i) const { return centers_.at(i); }

const Face& Mesh1D::face(std::size_t f) const { return faces_.at(f); }

std::vector<Face>::const_iterator Mesh1D::facesBegin() const { return faces_.begin(); }
std::vector<Face>::const_iterator Mesh1D::facesEnd() const { return faces_.end(); }

double Mesh1D::faceDistance(std::size_t f) const
{
    (void) f;
    return dx_;
}
double Mesh1D::distance(const Point& a, const Point& b) const
{
    double dx = a[0] - b[0];
    return std::abs(dx);
}
void Mesh1D::cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const
{
    nodes = {c, c + 1};
}

int Mesh1D::vtkCellType(std::size_t) const
{
    return 3; // VTK_LINE
}
