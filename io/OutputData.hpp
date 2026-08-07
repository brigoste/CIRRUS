#pragma once

#include <vector>

class MeshBase;
class ScalarField;
class PointField;

struct OutputData
{
    const MeshBase& mesh;

    const ScalarField& temperature;

    const PointField& reconstructedField;

    const std::vector<double>& rhs;

    const std::vector<double>& residual;
};