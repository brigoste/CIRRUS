#include "solver/preconditioners/SSORPreconditioner.hpp"

#include <stdexcept>

// ============================================================
// Construction
// ============================================================

SSORPreconditioner::SSORPreconditioner(double omega)
    : omega_(omega)
{
    if (omega_ <= 0.0 || omega_ >= 2.0) { throw std::invalid_argument("SSORPreconditioner: omega must satisfy 0 < omega < 2"); }
}

// ============================================================
// SETUP
// ============================================================

void SSORPreconditioner::setup(
    const LinearEquationSystem& sys)
{
    const std::size_t n = sys.size();

    if (n == 0) { throw std::invalid_argument("SSORPreconditioner::setup: system is empty"); }

    diagonal_.resize(n);

    for (std::size_t i = 0; i < n; ++i)
    {
        diagonal_[i] = sys.diagonal(i);

        if (diagonal_[i] == 0.0) { throw std::runtime_error("SSORPreconditioner::setup: zero diagonal coefficient"); }
    }

    system_ = &sys;
}

// ============================================================
// APPLY
//
// Computes:
//
//     z = M_SSOR^{-1} r
//
// where
//
//     M_SSOR = 1/[omega(2-omega)]
//              (D + omega L) D^-1 (D + omega U)
//
// The operation is performed as:
//
//     (1) (D + omega L) y = r
//     (2) w = D^-1 y
//     (3) (D + omega U) z = omega(2-omega) w
//
// ============================================================

void SSORPreconditioner::apply(
    const std::vector<double>& r,
    std::vector<double>& z) const
{
    if (system_ == nullptr) { throw std::runtime_error( "SSORPreconditioner::apply: preconditioner has not been setup"); }

    const std::size_t n = system_->size();

    if (r.size() != n) { throw std::runtime_error( "SSORPreconditioner::apply: vector size mismatch"); }

    const double scale = omega_ * (2.0 - omega_);

    // --------------------------------------------------------
    // Forward sweep:
    //
    //     (D + omega L) w = scale * r
    //
    //     w_i = [
    //         scale * r_i
    //         - omega * sum(A_ij w_j, j < i)
    //     ] / D_i
    // --------------------------------------------------------

    std::vector<double> w(n, 0.0);

    for (std::size_t i = 0; i < n; ++i)
    {
        double value = scale * r[i];

        for (const auto& [j, aij] : system_->row(i))
        {
            if (j < i) { value -= omega_ * aij * w[j]; }
        }

        w[i] = value / diagonal_[i];
    }

    // --------------------------------------------------------
    // Diagonal multiplication:
    //
    //     q = D w
    // --------------------------------------------------------

    std::vector<double> q(n, 0.0);

    for (std::size_t i = 0; i < n; ++i)
    {
        q[i] = diagonal_[i] * w[i];
    }

    // --------------------------------------------------------
    // Backward sweep:
    //
    //     (D + omega U) z = q
    //
    //     z_i = [
    //         q_i
    //         - omega * sum(A_ij z_j, j > i)
    //     ] / D_i
    // --------------------------------------------------------

    z.assign(n, 0.0);

    for (std::size_t i = n; i-- > 0;)
    {
        double value = q[i];

        for (const auto& [j, aij] : system_->row(i))
        {
            if (j > i) { value -= omega_ * aij * z[j]; }
        }

        z[i] = value / diagonal_[i];
    }
}
