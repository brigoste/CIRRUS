#pragma once

#include <filesystem>

#include "mesh/MeshBase.hpp"
#include "fields/ScalarField.hpp"

class VTKWriter
{
public:

    static void writeVTK(
        const MeshBase& mesh,
        const ScalarField& field,
        const std::filesystem::path& filename);

};
