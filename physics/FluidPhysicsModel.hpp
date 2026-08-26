class FluidPhysicsModel
{
public:
    virtual ~FluidPhysicsModel() = default;

    // =====================================================
    // Primary fields
    // =====================================================
    virtual FieldName velocityField() const = 0;
    virtual FieldName pressureField() const = 0;

    virtual void initializeFields(
        FieldRegistry& fields,
        const MeshBase& mesh) const;

    // =====================================================
    // Momentum equation
    // =====================================================
    virtual double momentumDiffusionCoefficient() const = 0;

    virtual double momentumConvectionFlux(
        const Face& face,
        const FieldRegistry& fields) const = 0;

    virtual Vector3 momentumSource(
        const MeshBase& mesh,
        std::size_t cell,
        const FieldRegistry& fields) const = 0;

    // =====================================================
    // Pressure / continuity
    // =====================================================
    virtual double pressureReferenceValue() const = 0;

    // =====================================================
    // Boundary conditions
    // =====================================================
    virtual Vector3 reconstructVelocityBoundaryValue(
        const BoundaryPatchSystem::Condition& bc,
        const Vector3& cellVelocity,
        double dx,
        bool isLeft) const = 0;

    virtual double reconstructPressureBoundaryValue(
        const BoundaryPatchSystem::Condition& bc,
        double cellPressure,
        double dx,
        bool isLeft) const = 0;
};
