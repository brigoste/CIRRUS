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

    static void writeCSVDebug(
        const MeshBase& mesh,
        const std::vector<double>& phi,
        const std::vector<double>& rhs,
        const std::vector<double>& residual,
        const std::string& filename);
};
