#pragma once

#include <vector>
#include <string>
#include "postprocessing/BoundaryReconstructor.hpp"

class MeshBase;
struct Point;   // forward-declare if Point is only used by reference internally

class FieldWriter
{
public:
    static void writeCSVDebug(
        const Field1D& field,
        const std::vector<double>& rhs,
        const std::vector<double>& residual,
        const std::string& filename);
};
