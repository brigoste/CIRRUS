#include "fields/ScalarField.hpp"
#include "mesh/MeshBase.hpp"
#include "mesh/primitives/Point.hpp"
#include "io/FieldOutput.hpp"
#include "io/PointField.hpp"
#include "io/VTKWriter.hpp"
#include "io/OutputData.hpp"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <vector>

// --------------------------------------------------
// MAIN WRITER
// --------------------------------------------------
void VTKWriter::write(
    const OutputData& data,
    const std::filesystem::path& filename)
{
    if (filename.has_parent_path())
    {
        std::filesystem::create_directories(
            filename.parent_path());
    }

    std::ofstream f(filename);

    if (!f.is_open())
    {
        throw std::runtime_error(
            "Failed to open VTU file: " + filename.string());
    }

    const auto& mesh  = data.mesh;

    const std::size_t Nnodes = mesh.nnodes();
    const std::size_t Ncells = mesh.ncells();

    f << "<?xml version=\"1.0\"?>\n";
    f << "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\">\n";
    f << "<UnstructuredGrid>\n";

    f << "<Piece NumberOfPoints=\"" << Nnodes
      << "\" NumberOfCells=\"" << Ncells << "\">\n";


    // =========================================================
    // POINTS
    // =========================================================
    f << "<Points>\n";
    f << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";

    for (std::size_t i = 0; i < Nnodes; ++i)
    {
        const Point& p = mesh.node(i);

        f << p.x[0] << " "
          << p.x[1] << " "
          << p.x[2] << "\n";
    }

    f << "</DataArray>\n";
    f << "</Points>\n";


    // =========================================================
    // CELLS
    // =========================================================
    std::vector<std::size_t> connectivity;
    std::vector<std::size_t> offsets;
    std::vector<int> types;

    // Upper bound for structured quad/hex meshes
    connectivity.reserve(Ncells * 8);
    offsets.reserve(Ncells);
    types.reserve(Ncells);

    std::size_t runningOffset = 0;

    for (std::size_t c = 0; c < Ncells; ++c)
    {
        std::vector<std::size_t> nodes;

        mesh.cellNodes(c, nodes);

        const int vtkType = mesh.vtkCellType(c);

        for (auto n : nodes)
        {
            connectivity.push_back(n);
        }

        runningOffset += nodes.size();

        offsets.push_back(runningOffset);
        types.push_back(vtkType);
    }


    f << "<Cells>\n";


    // -------------------------
    // Connectivity
    // -------------------------
    f << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";

    for (auto n : connectivity)
    {
        f << n << " ";
    }

    f << "\n</DataArray>\n";


    // -------------------------
    // Offsets
    // -------------------------
    f << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";

    for (auto o : offsets)
    {
        f << o << "\n";
    }

    f << "</DataArray>\n";


    // -------------------------
    // Types
    // -------------------------
    f << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";

    for (auto t : types)
    {
        f << t << "\n";
    }

    f << "</DataArray>\n";

    f << "</Cells>\n";


    // =========================================================
    // CELL DATA
    // =========================================================

    f << "<CellData>\n";

    for (const auto& outputField : data.fields)
    {
        if (outputField.cellField == nullptr)
        {
            continue;
        }

        const auto& field = *outputField.cellField;

        f << "<DataArray type=\"Float64\" Name=\""
        << outputField.name
        << "\" format=\"ascii\">\n";

        for (std::size_t c = 0; c < Ncells; ++c)
        {
            f << field[c] << "\n";
        }

        f << "</DataArray>\n";
    }

    f << "</CellData>\n";

    // =========================================================
    // POINT DATA
    // =========================================================

    f << "<PointData>\n";

    for (const auto& outputField : data.fields)
    {
        if (outputField.pointField == nullptr)
        {
            continue;
        }

        const auto& field = *outputField.pointField;

        f << "<DataArray type=\"Float64\" Name=\""
        << outputField.name
        << "\" format=\"ascii\">\n";

        for (std::size_t i = 0; i < field.phi.size(); ++i)
        {
            f << field.phi[i] << "\n";
        }

        f << "</DataArray>\n";
    }

    f << "</PointData>\n";


    f << "</Piece>\n";
    f << "</UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}