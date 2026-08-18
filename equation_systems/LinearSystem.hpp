#pragma once

#include <cstddef>
#include <utility>
#include <vector>

#include "equation_systems/LinearEquationSystem.hpp"

// ============================================================
// Sparse scalar linear system
//
// Concrete implementation of EquationSystem for:
//
//     A x = b
//
// Matrix storage is row-based and optimized for sparse FV
// systems.
// ============================================================

class LinearSystem : public LinearEquationSystem
{
public:
    LinearSystem() = default;
    explicit LinearSystem(std::size_t n);

    void resize(std::size_t n);

    std::size_t size() const override;

    void addCoeff(std::size_t i, std::size_t j, double val) override;

    void addRHS(std::size_t i, double val) override;

    void setRHS(std::size_t i, double val);

    double rhs(std::size_t i) const;

    double coeff(std::size_t i, std::size_t j) const;

    std::vector<double>& RHS();
    const std::vector<double>& RHS() const;

    void clear() override;

    const std::vector<std::pair<std::size_t, double>>&
    row(std::size_t i) const;

    double diagonal(std::size_t i) const override;

    std::size_t nnz() const;

    void matvec( const std::vector<double>& x, std::vector<double>& y ) const override;


private:
    std::size_t n_ = 0;
    std::vector<std::vector<std::pair<std::size_t, double>>> A_;
    std::vector<double> b_;
};
