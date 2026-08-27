#include "physics/ScalarTransport/ScalarTransportModel.hpp"

#include "fields/FieldRegistry.hpp"
#include "fields/FieldLocation.hpp"
#include "mesh/MeshBase.hpp"
#include "fields/FieldLocation.hpp"

void ScalarTransportModel::initializeFields(
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
