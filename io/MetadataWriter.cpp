#include "io/MetadataWriter.hpp"

#include "io/OutputData.hpp"
#include "io/PointField.hpp"

#include "mesh/MeshBase.hpp"
#include "fields/ScalarField.hpp"

#include "nlohmann/json.hpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <stdexcept>


void MetadataWriter::write(
    const OutputData& data,
    const std::filesystem::path& filename)
{
    if (filename.has_parent_path()) { std::filesystem::create_directories(filename.parent_path()); }

    nlohmann::json j;

    j["metadata_version"] = 1;

    const auto& mesh = data.mesh;

    // ------------------------------------
    // Mesh information
    // ------------------------------------
    j["mesh"]["nodes"] = mesh.nnodes();
    j["mesh"]["cells"] = mesh.ncells();
    j["mesh"]["faces"] = mesh.nfaces();

    // ------------------------------------
    // Field information
    // ------------------------------------
    j["fields"] = nlohmann::json::array();

    for (const auto& outputField : data.fields)
    {
        nlohmann::json field;

        field["name"] = outputField.name;

        if (outputField.cellField)
        {
            field["location"] = "cell";
            field["size"] = outputField.cellField->size();
        }
        else if (outputField.pointField)
        {
            field["location"] = "point";
            field["size"] = outputField.pointField->size();
        }

        j["fields"].push_back(field);
    }

    // ------------------------------------
    // Residual information
    // ------------------------------------
    double residualL2 = 0.0;
    double residualLinf = 0.0;

    for (double r : data.residual)
    {
        residualL2 += r * r;
        residualLinf = std::max( residualLinf, std::abs(r));
    }

    if (!data.residual.empty()) { residualL2 = std::sqrt( residualL2 / static_cast<double>(data.residual.size())); }

    j["residual"]["l2_norm"] = residualL2;
    j["residual"]["linf_norm"] = residualLinf;

    // ------------------------------------
    // Write file
    // ------------------------------------
    std::ofstream out(filename);

    if (!out.is_open()) { throw std::runtime_error( "Failed to open metadata output: " + filename.string()); }

    out << j.dump(4);
}
