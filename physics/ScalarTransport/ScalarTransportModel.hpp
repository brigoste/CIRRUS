#pragma once

#include "mesh/primitives/Face.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "physics/PhysicsModel.hpp"

class FieldRegistry;
class MeshBase;

class ScalarTransportModel : public PhysicsModel
{
public:
    virtual ~ScalarTransportModel() = default;

    virtual double initialSolutionValue() const = 0;

    virtual void initializeFields( FieldRegistry& fields, const MeshBase& mesh ) const;

    // =====================================================
    // Transport coefficients
    // =====================================================

    virtual double diffusionFaceCoefficient( const Face& face ) const = 0;

    virtual double convectionFaceFlux( const Face& face ) const = 0;

    virtual double diffusionCoefficient() const = 0;

    virtual double reconstructBoundaryValue( const BoundaryPatchSystem::Condition& bc, double phiCell, double dx, bool isLeft ) const = 0;

    // =====================================================
    // Source term
    // =====================================================

    virtual double cellSource( const MeshBase& mesh, std::size_t cell ) const = 0;
};