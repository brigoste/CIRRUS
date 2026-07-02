#pragma once

#include <vector>
#include <unordered_map>
#include <cstddef>
#include <stdexcept>

// ============================================================
// Sparse FV linear system (row-based hash sparse matrix)
// ============================================================

class LinearSystem
{
public:
    LinearSystem() = default;
    explicit LinearSystem(std::size_t n);

    void resize(std::size_t n);
    std::size_t size() const;

    // -------------------------
    // Matrix assembly
    // -------------------------
    void addCoeff(std::size_t i, std::size_t j, double val);

    // RHS
    void addRHS(std::size_t i, double val);
    void setRHS(std::size_t i, double val);

    // access  -- Scalar solvers only
    double rhs(std::size_t i) const;
    double coeff(std::size_t i, std::size_t j) const;

    // solver interface -- Assembly + vector math
    std::vector<double>& RHS();
    const std::vector<double>& RHS() const;

    void clear();

    // direct matrix access for solvers (GS/SOR/CG)
    const std::unordered_map<std::size_t, double>& row(std::size_t i) const;
    double diagonal(std::size_t i) const { return coeff(i, i); }

private:
    std::size_t n_ = 0;
    // A stored as: row -> (col -> value)
    std::vector<std::unordered_map<std::size_t, double>> A_;
    std::vector<double> b_;
};
