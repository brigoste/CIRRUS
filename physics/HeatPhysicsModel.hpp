#pragma once

#include "physics/PhysicsModel.hpp"
#include "mesh/primitives/Face.hpp"
#include "fields/FieldNames.hpp"

class FieldRegistry;

class HeatPhysicsModel : public PhysicsModel
{
public:
    explicit HeatPhysicsModel(double k)
        : k_(k) {}

    FieldName solutionField() const override                                                              { return FieldName::Temperature; }
    double initialSolutionValue() const override                                                          { return 300.0;  }

    double diffusionFaceCoefficient(const Face& face) const override;
    double convectionFaceFlux(const Face& face) const override;

    double diffusionCoefficient() const override                                                           { return k_; }

    double reconstructBoundaryValue( const BoundaryPatchSystem::Condition& bc, double phiCell, double dx, bool isLeft) const override;

    double cellSource( const MeshBase& mesh, std::size_t c) const override;

private:
    double k_;
};
