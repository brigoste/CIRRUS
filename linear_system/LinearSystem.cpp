#include "linear_system/LinearSystem.hpp"
#include <algorithm>

LinearSystem::LinearSystem(std::size_t n) { resize(n); }

void LinearSystem::resize(std::size_t n)
{
    n_ = n;
    A_.assign(n, {});
    b_.assign(n, 0.0);
}

std::size_t LinearSystem::size() const { return n_; }

// ============================================================
// MATRIX ASSEMBLY
// ============================================================

// IMPORTANT: FV accumulation semantics
// void LinearSystem::addCoeff(std::size_t i, std::size_t j, double val) { A_[i][j] += val; } 
void LinearSystem::addCoeff(
    std::size_t i,
    std::size_t j,
    double val)
{
    auto& row = A_[i];

    for (auto& [col, value] : row)
    {
        if (col == j)
        {
            value += val;
            return;
        }
    }

    row.emplace_back(j,val);
}
void LinearSystem::addRHS(std::size_t i, double val) { b_[i] += val; }

void LinearSystem::setRHS(std::size_t i, double val) { b_[i] = val; }

// ============================================================
// ACCESSORS
// ============================================================

double LinearSystem::rhs(std::size_t i) const { return b_[i]; }

// double LinearSystem::coeff(std::size_t i, std::size_t j) const
// {
//     auto it = A_[i].find(j);
//     return (it != A_[i].end()) ? it->second : 0.0;
// }
double LinearSystem::coeff(
    std::size_t i,
    std::size_t j) const
{
    for (const auto& [col,value] : A_[i])
    {
        if (col == j)
            return value;
    }

    return 0.0;
}

std::vector<double>& LinearSystem::RHS() { return b_; }

const std::vector<double>& LinearSystem::RHS() const { return b_; }

// const std::unordered_map<std::size_t, double>& LinearSystem::row(std::size_t i) const { return A_[i]; }
const std::vector<std::pair<std::size_t,double>>& LinearSystem::row(std::size_t i) const { return A_[i]; }

// void LinearSystem::clear()
// {
//     for (auto& row : A_) row.clear();
    
//     std::fill(b_.begin(), b_.end(), 0.0);
// }
void LinearSystem::clear()
{
    for(auto& row : A_) { row.clear(); }

    std::fill( b_.begin(), b_.end(), 0.0 );
}

std::size_t LinearSystem::nnz() const
{
    std::size_t count = 0;

    for (const auto& row : A_)
    {
        count += row.size();
    }

    return count;
}
