#pragma once

#include "io/FieldOutput.hpp"
#include <vector>
#include <memory>

class MeshBase;
class ScalarField;
class PointField;

// Non-owning view of simulation data for output writers.
// Referenced objects must remain alive during the write operation.
struct OutputData
{
    const MeshBase& mesh;
    std::vector<FieldOutput> fields;

    std::shared_ptr<PointField> reconstructedField;

    const std::vector<double>& rhs;
    const std::vector<double>& residual;
};