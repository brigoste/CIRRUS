#pragma once

#include "physics/PhysicsModel.hpp"
#include "mesh/primitives/Face.hpp"

class HeatPhysicsModel : public PhysicsModel
{
public:
    explicit HeatPhysicsModel(double k)
        : k_(k) {}

    double diffusionFaceCoefficient(const Face& face) const override;
    double convectionFaceFlux(const Face& face) const override;

    double diffusionCoefficient() const override { return k_; }

    double reconstructBoundaryValue( const BoundaryPatchSystem::Condition& bc, double phiCell, double dx, bool isLeft) const override;

    void addCellSources( const MeshBase& mesh, std::size_t c, FluxAccumulator& flux) const override;

    double cellSource( const MeshBase& mesh, std::size_t c) const override;

private:
    double k_ = 1.0;
};
