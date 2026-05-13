#pragma once

#include "mesh/MeshBase.hpp"
#include <vector>
#include <filesystem>
#include <string>

class VTKWriter
{
public:
    static void writeVTU(
        const MeshBase& mesh,
        const std::vector<double>& field,
        const std::filesystem::path& filename);
};
