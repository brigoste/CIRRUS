#include "mesh/QuadMesh2D.hpp"
#include <stdexcept>
#include <cmath>

QuadMesh2D::QuadMesh2D(std::size_t nx, std::size_t ny,
                       double lx, double ly)
: nx_(nx), ny_(ny)
{
    if (nx < 2 || ny < 2)
        throw std::runtime_error("Invalid mesh size");

    dx_ = lx / nx;
    dy_ = ly / ny;

    cells_.resize(nx_ * ny_);
    nodes_.resize((nx_+1)*(ny_+1));

    boundaryGroups_ = {}; // LEFT, RIGHT, TOP, BOTTOM

    buildCells();
    buildFaces();
    buildNodes();
}
std::size_t QuadMesh2D::idx(std::size_t i, std::size_t j) const
{
    return j * nx_ + i;
}
void QuadMesh2D::buildCells()
{
    for (std::size_t j = 0; j < ny_; ++j)
    {
        for (std::size_t i = 0; i < nx_; ++i)
        {
            std::size_t c = idx(i, j);

            cells_[c].id = c;

            cells_[c].center = Point{ (i+0.5) * dx_, (j+0.5) * dy_, 0.0 };

            cells_[c].faces.clear();
        }
    }
}
void QuadMesh2D::buildFaces()
{
    faces_.clear();

    auto addFace = [&](std::size_t owner,
                   std::size_t neighbor,
                   const Point& c,
                   const Point& n,
                   double area,
                   const Point& dPN)
    {
        Face f;
        f.owner = owner;
        f.neighbor = neighbor;
        f.center = c;
        f.normal = n;
        f.area = area;
        f.dPN = dPN;

        faces_.push_back(f);

        cells_[owner].faces.push_back(faces_.size() - 1);
        if (neighbor != Face::INVALID) cells_[neighbor].faces.push_back(faces_.size() - 1);
    };

    // -------------------------
    // X-direction faces
    // -------------------------
    for (std::size_t j = 0; j < ny_; ++j)
    {
        for (std::size_t i = 0; i < nx_ - 1; ++i)
        {
            std::size_t cL = idx(i, j);
            std::size_t cR = idx(i + 1, j);

            Point fc{ (i + 1.0) * dx_, (j + 0.5) * dy_, 0.0 };

            Point normal{1.0, 0.0, 0.0};

            Point dPN{ dx_, 0.0, 0.0 };

            addFace(cL, cR, fc, normal, dy_, dPN);
        }
    }

    // -------------------------
    // Y-direction faces
    // -------------------------
    for (std::size_t j = 0; j < ny_ - 1; ++j)
    {
        for (std::size_t i = 0; i < nx_; ++i)
        {
            std::size_t cB = idx(i, j);
            std::size_t cT = idx(i, j + 1);

            Point fc{ (i + 0.5) * dx_, (j + 1.0) * dy_, 0.0 };

            Point normal{0.0, 1.0, 0.0};

            Point dPN{ 0.0, dy_, 0.0 };

            addFace(cB, cT, fc, normal, dx_, dPN);
        }
    }

    // -------------------------
    // Boundary faces
    // -------------------------
    auto addBoundary = [&](std::size_t owner,
                       const Point& c,
                       const Point& n,
                       double area,
                       const Point& dPN) -> std::size_t
    {
        Face f;

        f.owner = owner;
        f.neighbor = Face::INVALID;
        f.center = c;
        f.normal = n;
        f.area = area;
        f.dPN = dPN;

        faces_.push_back(f);

        std::size_t idx = faces_.size() - 1;

        cells_[owner].faces.push_back(idx);

        return idx;
    };
    
    // LEFT BOUNDARY
    for (std::size_t j = 0; j < ny_; ++j)
    {
        std::size_t f = addBoundary(
            idx(0, j),
            Point{0.0, (j + 0.5) * dy_, 0.0},
            Point{-1.0, 0.0, 0.0},
            dy_,
            Point{-dx_/2, 0.0, 0.0}
        );

        boundaryGroups_[toGroup(Patch::LEFT)].push_back(f);
    }

    // RIGHT BOUNDARY
    for (std::size_t j = 0; j < ny_; ++j)
    {
        std::size_t f = addBoundary(
            idx(nx_ - 1, j),
            Point{nx_ * dx_, (j + 0.5) * dy_, 0.0},
            Point{1.0, 0.0, 0.0},
            dy_,
            Point{dx_/2, 0.0, 0.0}
        );

        boundaryGroups_[toGroup(Patch::RIGHT)].push_back(f);
    }

    // TOP BOUNDARY
    for (std::size_t i = 0; i < nx_; ++i)
    {
        std::size_t f = addBoundary(
            idx(i, ny_ - 1),
            Point{(i + 0.5) * dx_, ny_ * dy_, 0.0},
            Point{0.0, 1.0, 0.0},
            dx_,
            Point{0.0, dy_/2, 0.0}
        );

        boundaryGroups_[toGroup(Patch::TOP)].push_back(f);
    }

    // BOTTOM BOUNDARY
    for (std::size_t i = 0; i < nx_; ++i)
    {
        std::size_t f = addBoundary( 
                    idx(i, 0), 
                    Point{(i + 0.5) * dx_, 0.0, 0.0}, 
                    Point{0.0, -1.0, 0.0}, 
                    dx_, 
                    Point{0.0, -dy_/2, 0.0} );

        boundaryGroups_[toGroup(Patch::BOTTOM)].push_back(f);
    }
}

void QuadMesh2D::buildNodes(){
    for (std::size_t j = 0; j <= ny_; ++j)
    {
        for (std::size_t i = 0; i <= nx_; ++i)
        {
            nodes_[j*(nx_+1)+i] = Point{i*dx_, j*dy_, 0.0};
        }
    }
}

double QuadMesh2D::distance( const Point& a,
                             const Point& b) const 
{
    const double dx = a[0] - b[0];
    const double dy = a[1] - b[1];

    return std::sqrt(dx*dx + dy*dy);
}
void QuadMesh2D::cellNodes( std::size_t c,
                            std::vector<std::size_t>& nodes) const
{
    std::size_t i = c % nx_;
    std::size_t j = c / nx_;

    std::size_t n0 = j * (nx_ + 1) + i;
    std::size_t n1 = n0 + 1;
    std::size_t n2 = n1 + (nx_ + 1);
    std::size_t n3 = n0 + (nx_ + 1);

    nodes = {n0, n1, n2, n3};
}

#ifdef DEBUG
for (const auto& face : faces_)
{
    double d_eff =
        std::abs(LA::dot(face.dPN, face.normal));

    if (d_eff <= 0.0)
        throw std::runtime_error("Invalid face spacing");
}
#endif
