#include "equation_systems/LinearSystem.hpp"

#include <algorithm>

// ============================================================
// Construction
// ============================================================

LinearSystem::LinearSystem(std::size_t n)
{
    resize(n);
}

// ============================================================
// SIZING
// ============================================================

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

void LinearSystem::addCoeff(
    std::size_t i,
    std::size_t j,
    double val)
{
    auto& row = A_[i];

    // FV assembly requires accumulation.
    for (auto& [col, value] : row)
    {
        if (col == j)
        {
            value += val;
            return;
        }
    }

    row.emplace_back(j, val);
}

// ============================================================
// RHS ASSEMBLY
// ============================================================

void LinearSystem::addRHS(
    std::size_t i,
    double val)
{
    b_[i] += val;
}

void LinearSystem::setRHS(
    std::size_t i,
    double val)
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

double LinearSystem::coeff(
    std::size_t i,
    std::size_t j) const
{
    for (const auto& [col, value] : A_[i])
    {
        if (col == j)
        {
            return value;
        }
    }

    return 0.0;
}

std::vector<double>& LinearSystem::RHS()
{
    return b_;
}

const std::vector<double>& LinearSystem::RHS() const
{
    return b_;
}

// ============================================================
// SPARSE MATRIX ACCESS
// ============================================================

const std::vector<std::pair<std::size_t, double>>&
LinearSystem::row(std::size_t i) const
{
    return A_[i];
}

double LinearSystem::diagonal(std::size_t i) const
{
    return coeff(i, i);
}

// ============================================================
// RESET
// ============================================================

void LinearSystem::clear()
{
    for (auto& row : A_)
    {
        row.clear();
    }

    std::fill(
        b_.begin(),
        b_.end(),
        0.0
    );
}

// ============================================================
// SPARSE MATRIX INFORMATION
// ============================================================

std::size_t LinearSystem::nnz() const
{
    std::size_t count = 0;

    for (const auto& row : A_)
    {
        count += row.size();
    }

    return count;
}