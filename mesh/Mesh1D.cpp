#include "mesh/Mesh1D.hpp"
#include "mesh/primitives/Point.hpp"
#include <stdexcept>
#include <cassert>
#include <cmath>

#include <iostream>

// ===============================
// Constructor
// ===============================
Mesh1D::Mesh1D(std::size_t N, double L)
    : N_(N), L_(L)
{
    if (N_ == 0) 
    { 
        throw std::runtime_error("Mesh1D: N must be > 0"); 
    }

    dx_ = L_ / static_cast<double>(N_);

    nodes_.resize(N_ + 1);
    centers_.resize(N_);
    faces_.resize(N_ + 1);
    cells_.resize(N_);

    boundaryGroups_ = {}; // LEFT, RIGHT
    
    // ---------------------------
    // Nodes
    // ---------------------------
    for (std::size_t i = 0; i <= N_; ++i) 
    { 
        nodes_[i].x[0] = i * dx_; 
    }

    // ---------------------------
    // Cell centers
    // ---------------------------
    for (std::size_t i = 0; i < N_; ++i) 
    { 
        centers_[i].x[0] = (i + 0.5) * dx_; 
    }

    // ---------------------------
    // Faces
    // ---------------------------
    for (std::size_t f = 0; f <= N_; ++f)
    {
        Face& face = faces_[f];

        face.center.x[0] = f * dx_;
        face.area = 1.0;

        if (f == 0)
        {
            face.owner = 0;
            face.neighbor = Face::INVALID;
            face.normal = Vector(-1.0, 0.0, 0.0);

            face.dPN = face.center - centers_[0];

            boundaryGroups_[toGroup(Patch::LEFT)].push_back(f);
        }
        else if (f == N_)
        {
            face.owner = N_ - 1;
            face.neighbor = Face::INVALID;
            face.normal = Vector(1.0, 0.0, 0.0);

            face.dPN = face.center - centers_[N_ - 1];

            boundaryGroups_[toGroup(Patch::RIGHT)].push_back(f);
        }
        else
        {
            face.owner = f - 1;
            face.neighbor = f;
            face.normal = Vector(1.0, 0.0, 0.0);

            face.dPN = centers_[f] - centers_[f-1];
        }
    }

    // ---------------------------
    // Sanity check
    // ---------------------------
    if (faces_.size() != N_ + 1) 
    { 
        throw std::runtime_error("Invalid 1D mesh topology"); 
    }

    for (std::size_t f = 1; f < N_; ++f)
    {
        assert(faces_[f].owner == f - 1);
        assert(faces_[f].neighbor == f);
    }
}

// =========================================================
// MeshBase interface
// =========================================================

std::size_t Mesh1D::ncells() const                                             { return N_; }
std::size_t Mesh1D::nnodes() const                                             { return nodes_.size(); }
std::size_t Mesh1D::nfaces() const                                             { return faces_.size(); }

const Point& Mesh1D::node(std::size_t i) const                                 { return nodes_.at(i); }
const Point& Mesh1D::cellCenter(std::size_t i) const                           { return centers_.at(i); }

const Face& Mesh1D::face(std::size_t f) const                                  { return faces_.at(f); }

std::vector<Face>::const_iterator Mesh1D::facesBegin() const                   { return faces_.begin(); }
std::vector<Face>::const_iterator Mesh1D::facesEnd() const                     { return faces_.end(); }

// ---------------------------
// Geometry
// ---------------------------
double Mesh1D::faceDistance(std::size_t f) const                               { return faces_.at(f).dPN.magnitude(); } 

double Mesh1D::distance(const Point& a, const Point& b) const                  { return std::abs(a[0] - b[0]); }

std::size_t Mesh1D::nBoundaryGroups() const                                    { return boundaryGroups_.size(); }

const std::vector<std::size_t>& Mesh1D::boundaryFaces(std::size_t group) const { return boundaryGroups_.at(group); }

// ---------------------------
// Connectivity
// ---------------------------
void Mesh1D::cellNodes(std::size_t c, std::vector<std::size_t>& nodes) const   { nodes = {c, c + 1}; }

const Cell& Mesh1D::cell(std::size_t i) const                                  { return cells_.at(i); }

int Mesh1D::vtkCellType(std::size_t) const                                     { return 3; } // VTK_LINE 


std::size_t Mesh1D::findCell(const Point& position) const
{
    const double x = position[0];
    const double length = ncells() * dx_;

    constexpr double eps = 1.0e-12;

    if (x < -eps || x > length + eps)
    {
        throw std::out_of_range(
            "Mesh1D::findCell: position lies outside mesh."
        );
    }

    if (x >= length - eps)
    {
        return ncells() - 1;
    }

    return static_cast<std::size_t>(x / dx_);
}
