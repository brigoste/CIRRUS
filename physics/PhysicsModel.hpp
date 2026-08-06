#pragma once

#include "mesh/primitives/Face.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "discretization/FluxAccumulator.hpp"
#include "fields/FieldNames.hpp"

class VerificationCase; // forward declare
class FieldRegistry;
class MeshBase;

class PhysicsModel
{
public:
    virtual ~PhysicsModel() = default;

    virtual FieldName solutionField() const = 0;
    virtual double initialSolutionValue() const = 0;

    virtual void initializeFields( FieldRegistry& fields, const MeshBase& mesh) const;

    // =====================================================
    // Transport coefficients
    // =====================================================
    virtual double diffusionFaceCoefficient(const Face& face) const = 0;
    virtual double convectionFaceFlux(const Face& face) const = 0;

    virtual double diffusionCoefficient() const = 0;

    // =====================================================
    // Boundary handling
    // =====================================================
    virtual double boundaryDirichletValue( const BoundaryPatchSystem::Condition& bc, const Face& face) const
    {
        (void)face;
        return bc.value;
    }

    virtual double reconstructBoundaryValue( const BoundaryPatchSystem::Condition& bc, double phiCell, double dx, bool isLeft) const = 0;

    // =====================================================
    // Source term (UNIFIED ENTRY POINT)
    // =====================================================
    virtual double cellSource( const MeshBase& mesh, std::size_t cell) const = 0;

    virtual void addCellSources( const MeshBase& mesh, std::size_t cell, FluxAccumulator& flux) const = 0;

    // =====================================================
    // Optional: manufactured forcing hook
    // =====================================================
    void attachVerification(const VerificationCase* vc)                                             { vc_ = vc; }

protected:
    const VerificationCase* vc_ = nullptr;
};
