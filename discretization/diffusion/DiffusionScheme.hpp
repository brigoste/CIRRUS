#pragma once

#include "discretization/FaceDiffusion.hpp"
#include "equation_systems/EquationSystem.hpp"

class DiffusionScheme
{
public:
    virtual ~DiffusionScheme() = default;

    virtual void assemble(
        const FaceDiffusion& face,
        EquationSystem& sys
    ) const = 0;
};