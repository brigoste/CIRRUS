#include "solver/preconditioners/JacobiPreconditioner.hpp"

#include <cmath>
#include <stdexcept>


void JacobiPreconditioner::setup(
    const LinearSystem& sys)
{
    const std::size_t N = sys.size();

    diagInv_.resize(N);

    for (std::size_t i = 0; i < N; ++i)
    {
        const double aii = sys.diagonal(i);

        if (std::abs(aii) < 1e-30) { throw std::runtime_error( "Jacobi preconditioner: zero diagonal."); }

        diagInv_[i] = 1.0 / aii;
    }
}


void JacobiPreconditioner::apply(
    const std::vector<double>& r,
    std::vector<double>& z) const
{
    const std::size_t N = r.size();

    z.resize(N);

    for (std::size_t i = 0; i < N; ++i) { z[i] = diagInv_[i] * r[i]; }
}
