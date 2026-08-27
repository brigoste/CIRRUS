#pragma once

#include "fields/FieldNames.hpp"

class FieldRegistry;
class MeshBase;
class VerificationCase;

class PhysicsModel
{
public:
    virtual ~PhysicsModel() = default;

    // =====================================================
    // Model identity
    // =====================================================

    virtual FieldName solutionField() const = 0;

    // =====================================================
    // Field initialization
    // =====================================================

    virtual void initializeFields( FieldRegistry& fields, const MeshBase& mesh ) const;

    // =====================================================
    // Verification
    // =====================================================

    void attachVerification(const VerificationCase* vc) { vc_ = vc; }

protected:
    const VerificationCase* vc_ = nullptr;
};