#include "mesh/Mesh1D.hpp"
#include "bc/BoundaryFace.hpp"

BoundaryFace Mesh1D::faceType(int i) const
{
    if (i == 0) return BoundaryFace::Left;
    if (i == n - 1) return BoundaryFace::Right;
    return BoundaryFace::Interior;
}

Mesh1D::Mesh1D(int n_, double L_, double A_, double k_)
    : n(n_), L(L_), A(A_), k(k_)
{
    dx = L / static_cast<double>(n - 1);

    x.resize(n);

    for (int i = 0; i < n; ++i) {
        x[i] = i * dx;
    }
}
