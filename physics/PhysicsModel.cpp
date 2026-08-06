#include "PhysicsModel.hpp"

#include "fields/FieldRegistry.hpp"
#include "fields/FieldLocation.hpp"
#include "mesh/MeshBase.hpp"
#include "fields/FieldLocation.hpp"

void PhysicsModel::initializeFields(
    FieldRegistry& fields,
    const MeshBase& mesh
) const
{
    fields.createScalar(
        solutionField(),
        mesh,
        FieldLocation::Cell,
        initialSolutionValue()
    );
}
