#pragma once

#include "physics/PhysicsModel.hpp"
#include "mesh/primitives/Face.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "fields/FieldNames.hpp"

class AdvectionDiffusionPhysicsModel : public PhysicsModel
{
public:
    AdvectionDiffusionPhysicsModel(
        double gamma,
        double rho,
        double ux,
        double uy,
        double uz)
        : gamma_(gamma),
          rho_(rho),
          ux_(ux),
          uy_(uy),
          uz_(uz)
        {}

    double diffusionFaceCoefficient(const Face& face) const override;
    double convectionFaceFlux(const Face& face) const override;

    double diffusionCoefficient() const override { return gamma_; }

    double reconstructBoundaryValue( const BoundaryPatchSystem::Condition& bc, double phiCell, double dx, bool isLeft) const override;

    void addCellSources( const MeshBase& mesh, std::size_t c, FluxAccumulator& flux) const override;
    
    double cellSource( const MeshBase& mesh, std::size_t cell) const override;

    FieldName solutionField() const override { return FieldName::Temperature; }
    double initialSolutionValue() const override { return 0.0; }

private:
    double gamma_ = 0.0;

    double rho_ = 1.0;
    double ux_  = 0.0;
    double uy_  = 0.0;
    double uz_  = 0.0;
};
