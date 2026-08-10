#pragma once

// #include "discretization/FluxAccumulator.hpp"
// #include "equation_systems/EquationSystem.hpp"

class MeshBase;
class FluxAccumulator;
class EquationSystem;

class Operator
{
public:

    virtual ~Operator() = default;

    virtual void assemble(
        const MeshBase& mesh,
        const FluxAccumulator& flux,
        EquationSystem& sys
    ) const = 0;
};