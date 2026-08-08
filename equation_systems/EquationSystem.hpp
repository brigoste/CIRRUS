#pragma once

#include <cstddef>

class EquationSystem
{
public:
    virtual ~EquationSystem() = default;

    // System size
    virtual std::size_t size() const = 0;

    // Matrix assembly
    virtual void addCoeff(std::size_t i, std::size_t j, double value) = 0;

    // RHS assembly
    virtual void addRHS(std::size_t i, double value) = 0;

    // Reset
    virtual void clear() = 0;
};