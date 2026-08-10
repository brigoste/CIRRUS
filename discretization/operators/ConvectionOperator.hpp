#pragma once

#include "discretization/operators/Operator.hpp"
#include "discretization/interpolators/InterpolationScheme.hpp"
#include "mesh/MeshBase.hpp"

class ConvectionOperator : public Operator
{
public:
    explicit ConvectionOperator(const InterpolationScheme& interpolation);

    void assemble( const MeshBase& mesh, const FluxAccumulator& flux, EquationSystem& sys ) const override;

private:
    const InterpolationScheme& interpolation_;
};