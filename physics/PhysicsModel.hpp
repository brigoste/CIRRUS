#pragma once

#include "mesh/primitives/Point.hpp"
#include "mesh/primitives/Face.hpp"
#include "mesh/MeshBase.hpp"
#include "discretization/FluxAccumulator.hpp"

class PhysicsModel
{
public:
    virtual ~PhysicsModel() = default;

    virtual double diffusionCoeff(const Face&) const = 0;
    virtual double convectionCoeff(double) const = 0;

    // NEW: boundary closure hooks
    virtual double diffusionScalar() const = 0;
    virtual double reconstructBoundaryValue(const BoundaryPatchSystem::Condition& bc,
                                            double phiCell,
                                            double dx,
                                            bool isLeft) const = 0;
    virtual void addCellSources(const MeshBase& mesh, 
                                std::size_t cell,
                                FluxAccumulator& flux) const = 0;
    // {
    //     // Can add source term per cell here. Only treat a single cell here as we loop in FluxBuilder.cpp 
    //     (void)mesh;
    //     (void)cell;
    //     (void)flux;
    // }
    
};
