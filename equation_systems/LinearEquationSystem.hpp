#pragma once

#include <vector>
#include <cstddef>

#include "equation_systems/EquationSystem.hpp"

class LinearEquationSystem : public EquationSystem
{
public:
    virtual ~LinearEquationSystem() = default;

    virtual double coeff( std::size_t i, std::size_t j ) const = 0;

    virtual double rhs( std::size_t i ) const = 0;
    virtual const std::vector<std::pair<std::size_t, double>>& row(std::size_t i) const = 0;

    virtual void matvec( const std::vector<double>& x, std::vector<double>& y ) const = 0;
};