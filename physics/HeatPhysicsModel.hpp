#pragma once

#include "PhysicsModel.hpp"
#include "mesh/primitives/Face.hpp"
#include "utils/LinearAlgebraUtils.hpp"

class HeatPhysicsModel : public PhysicsModel
{
public:
    explicit HeatPhysicsModel(double k, double Su, double Sp)
        : k_(k), Su_(Su), Sp_(Sp) {} 

    double diffusionScalar() const override
    {
        return k_;
    }
    double diffusionCoeff(const Face& face) const;
    double convectionCoeff(double flux) const;
    double reconstructBoundaryValue(const BoundaryPatchSystem::Condition& bc,
                                            double phiCell,
                                            double dx,
                                            bool isLeft) const override;
    void addCellSources(const MeshBase& mesh,
                        std::size_t c,
                        FluxAccumulator& flux) const override;

private:
    double k_ = 1.0;
    double Su_ = 0.0;
    double Sp_ = 0.0;

};
