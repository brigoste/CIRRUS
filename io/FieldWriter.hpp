#pragma once

#include "mesh/MeshBase.hpp"
#include <vector>
#include <string>

class FieldWriter
{
public:
    static void writeCSV(
        const MeshBase& mesh,
        const std::vector<double>& field,
        const std::string& filename);
};
