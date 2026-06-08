#pragma once

#include <vector>
#include <string>
#include "postprocessing/BoundaryReconstructor.hpp"
#include "io/PointField.hpp"

class MeshBase;
struct Point;   // forward-declare if Point is only used by reference internally

class FieldWriter
{
public:
    static void writeCSVDebug(
        const PointField& field,
        const std::vector<double>& rhs,
        const std::vector<double>& residual,
        const std::string& filename);
    };
