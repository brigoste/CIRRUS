#include "physics/HeatPhysicsModel.hpp"
#include "mesh/primitives/Face.hpp"
#include "utils/LinearAlgebraUtils.hpp"
#include "mesh/MeshBase.hpp"
#include "fields/FieldRegistry.hpp"

#include <cmath>
#include <stdexcept>
#include <iostream>

// void HeatPhysicsModel::initializeFields( FieldRegistry& fields, const MeshBase& mesh ) const
// {
//     fields.createScalar(
//         solutionField(),
//         mesh,
//         FieldLocation::Cell,
//         300.0
//     );
// }

double HeatPhysicsModel::diffusionFaceCoefficient(const Face& face) const
{
    const double d_eff = std::abs(LA::dot(face.dPN, face.normal));

    if (d_eff <= 0.0) 
    { 
        throw std::runtime_error("Invalid face spacing"); 
    }

    return k_ * face.area / d_eff;
}

double HeatPhysicsModel::convectionFaceFlux(const Face& /*face*/) const { return 0.0; }

double HeatPhysicsModel::reconstructBoundaryValue(
    const BoundaryPatchSystem::Condition& bc,
    double phiCell,
    double dx,
    bool /*isLeft*/) const
{
    const double k = diffusionCoefficient();
    
    switch (bc.type)
    {
        
        case bc::Type::Dirichlet:
            return bc.value;

        case bc::Type::Neumann:
            return phiCell + bc.flux * dx / k;

        case bc::Type::Robin:
            return (k * phiCell + bc.transferCoefficient * dx * bc.referenceValue) / (k + bc.transferCoefficient * dx);

        default:
            throw std::runtime_error("Unsupported BC type");
    }
}
double HeatPhysicsModel::cellSource(
    const MeshBase& mesh,
    std::size_t c) const
{
    (void) mesh;
    (void) c;
    return 0;   // note: Sp usually goes into diagonal, not RHS
}
