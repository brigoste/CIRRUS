#include "HeatSystem1D.hpp"
#include "discretization/Discretization1D.hpp"

HeatSystem1D::HeatSystem1D(int n, double L, double A, double k)
    : mesh_(n, L, A, k),
      coeffs_(n)
{
}

// ----------------------------------------------------
// Add boundary condition
// ----------------------------------------------------
void HeatSystem1D::addBC(std::unique_ptr<BoundaryCondition> bc)
{
    bcs_.push_back(std::move(bc));
}

// ----------------------------------------------------
// Assemble linear system
// ----------------------------------------------------
void HeatSystem1D::assemble()
{
    const Mesh1D& mesh = mesh_;
    Coefficients1D& c = coeffs_;

    int n = mesh.n;
    double dx = mesh.dx;
    double A  = mesh.A;
    double k  = mesh.k;

    // -------------------------------------------------
    // 1. RESET COEFFICIENTS
    // -------------------------------------------------
    for (int i = 0; i < n; ++i) {
        c.aP[i] = 0.0;
        c.aE[i] = 0.0;
        c.aW[i] = 0.0;
        c.b[i]  = 0.0;
    }

    // -------------------------------------------------
    // 2. INTERIOR DISCRETIZATION (1D diffusion)
    // -------------------------------------------------
    double aw = k * A / dx;
    double ae = k * A / dx;

    for (int i = 1; i < n - 1; ++i) {
        c.aW[i] = aw;
        c.aE[i] = ae;
        c.aP[i] = aw + ae;
        c.b[i]  = 0.0;
    }
    // -------------------------------------------------
    // 3. APPLY SOURCE TERMS (Linearized)
    // -------------------------------------------------
    for (int i = 0; i < n; ++i) {
        c.aP[i] -= c.Sp[i];
        c.b[i]  += c.Su[i];
    }

    // -------------------------------------------------
    // 4. APPLY BOUNDARY CONDITIONS
    // -------------------------------------------------
    for (const auto& bc : bcs_) {
        bc->apply(mesh, c);
    }
}

// ----------------------------------------------------
// Solve system using TDMA
// ----------------------------------------------------
std::vector<double> HeatSystem1D::solve()
{
    return TDMA(coeffs_);
}

// ----------------------------------------------------
// Accessors
// ----------------------------------------------------
const Mesh1D& HeatSystem1D::mesh() const {
    return mesh_;
}

const Coefficients1D& HeatSystem1D::coeffs() const {
    return coeffs_;
}

void HeatSystem1D::setSource(std::function<double(double)> Su_func,
                             std::function<double(double)> Sp_func)
{
    int n = mesh_.n;

    for (int i = 0; i < n; ++i) {
        double x = mesh_.x[i];
        coeffs_.Su[i] = Su_func ? Su_func(x) : 0.0;
        coeffs_.Sp[i] = Sp_func ? Sp_func(x) : 0.0;
    }
}

void HeatSystem1D::setConstantSource(double Su, double Sp)
{
    int n = mesh_.n;

    for (int i = 0; i < n; ++i) {
        coeffs_.Su[i] = Su;
        coeffs_.Sp[i] = Sp;
    }
}

void HeatSystem1D::clearSource()
{
    int n = mesh_.n;

    for (int i = 0; i < n; ++i) {
        coeffs_.Su[i] = 0.0;
        coeffs_.Sp[i] = 0.0;
    }
}
