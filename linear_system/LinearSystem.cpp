#include "linear_system/LinearSystem.hpp"
#include <stdexcept>
#include <algorithm>

// -----------------------------
// Constructor
// -----------------------------
LinearSystem::LinearSystem(int N)
    : N_(N),
      aP_(N, 0.0),
      b_(N, 0.0),
      x_(N, 0.0),
      x_old_(N, 0.0),
      A_(N),
      nbrs_(N)
{}

// -----------------------------
int LinearSystem::size() const
{
    return N_;
}

// -----------------------------
void LinearSystem::resize(int N)
{
    N_ = N;

    aP_.assign(N, 0.0);
    b_.assign(N, 0.0);
    x_.assign(N, 0.0);
    x_old_.assign(N, 0.0);

    A_.assign(N, {});
    nbrs_.assign(N, {});
}

// -----------------------------
// Sparse assembly
// -----------------------------
void LinearSystem::addCoeff(int i, int j, double a)
{
    if (i < 0 || i >= N_ || j < 0 || j >= N_)
        throw std::runtime_error("addCoeff: index out of range");

    for (auto& [col, val] : A_[i])
    {
        if (col == j)
        {
            val += a;
            return;
        }
    }

    A_[i].push_back({j, a});
    nbrs_[i].push_back(j);
}

// -----------------------------
void LinearSystem::addDiag(int i, double aP)
{
    if (i < 0 || i >= N_)
        throw std::runtime_error("addDiag: index out of range");

    aP_[i] += aP;
}

// -----------------------------
void LinearSystem::setRHS(int i, double b)
{
    if (i < 0 || i >= N_)
        throw std::runtime_error("setRHS: index out of range");

    b_[i] = b;
}

void LinearSystem::addRHS(int i, double b)
{
    if (i < 0 || i >= N_)
        throw std::runtime_error("addRHS: index out of range");

    b_[i] += b;
}

// -----------------------------
void LinearSystem::clearRow(int i)
{
    if (i < 0 || i >= N_)
        throw std::runtime_error("clearRow: index out of range");

    A_[i].clear();
    nbrs_[i].clear();
    aP_[i] = 0.0;
    b_[i] = 0.0;
}

// -----------------------------
// Access
// -----------------------------
const std::vector<std::pair<int,double>>& LinearSystem::row(int i) const
{
    return A_[i];
}

double LinearSystem::diag(int i) const
{
    return aP_[i];
}

const std::vector<int>& LinearSystem::neighbors(int i) const
{
    return nbrs_[i];
}

// -----------------------------
// Vectors
// -----------------------------
std::vector<double>& LinearSystem::rhs()
{
    return b_;
}

const std::vector<double>& LinearSystem::rhs() const
{
    return b_;
}

std::vector<double>& LinearSystem::solution()
{
    return x_;
}

const std::vector<double>& LinearSystem::solution() const
{
    return x_;
}

std::vector<double>& LinearSystem::solutionOld()
{
    return x_old_;
}
const std::vector<double>& LinearSystem::diagonal() const
{
    return aP_;
}
double LinearSystem::coeff(int i, int j) const
{
    if (i < 0 || i >= N_ || j < 0 || j >= N_)
        throw std::runtime_error("coeff: index out of range");

    for (const auto& [col, val] : A_[i])
        if (col == j)
            return val;

    return 0.0;
}
