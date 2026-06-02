#include "linear_system/LinearSystem.hpp"

LinearSystem::LinearSystem(std::size_t n)
{
    resize(n);
}

void LinearSystem::resize(std::size_t n)
{
    n_ = n;
    A_.assign(n, {});
    b_.assign(n, 0.0);
}

std::size_t LinearSystem::size() const
{
    return n_;
}

// ============================================================
// MATRIX ASSEMBLY
// ============================================================

void LinearSystem::addCoeff(std::size_t i, std::size_t j, double val)
{
    A_[i][j] += val;   // IMPORTANT: FV accumulation semantics
}

void LinearSystem::addRHS(std::size_t i, double val)
{
    b_[i] += val;
}

void LinearSystem::setRHS(std::size_t i, double val)
{
    b_[i] = val;
}

// ============================================================
// ACCESSORS
// ============================================================

double LinearSystem::rhs(std::size_t i) const
{
    return b_[i];
}

double LinearSystem::coeff(std::size_t i, std::size_t j) const
{
    auto it = A_[i].find(j);
    return (it != A_[i].end()) ? it->second : 0.0;
}

std::vector<double>& LinearSystem::RHS()
{
    return b_;
}

const std::vector<double>& LinearSystem::RHS() const
{
    return b_;
}

const std::unordered_map<std::size_t, double>& LinearSystem::row(std::size_t i) const
{
    return A_[i];
}

void LinearSystem::clear()
{
    for (auto& row : A_)
        row.clear();

    std::fill(b_.begin(), b_.end(), 0.0);
}
