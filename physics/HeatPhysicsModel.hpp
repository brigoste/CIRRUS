#pragma once

#include "PhysicsModel.hpp"
#include "mesh/Face.hpp"

class HeatPhysicsModel : public PhysicsModel
{
public:
    explicit HeatPhysicsModel(double k)
        : k_(k) {} 

    double wallConductivity() const override
    {
        return k_;
    }
    double diffusionCoeff(const Face& face, double d) const;
    double convectionCoeff(double flux) const;
    double reconstructBoundaryValue(const BoundaryPatchSystem::Condition& bc,
                                            double phiCell,
                                            double dx,
                                            bool isLeft) const override;

private:
    double k_ = 1.0;

};
