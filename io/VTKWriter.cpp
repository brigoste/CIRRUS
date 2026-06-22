#include "io/VTKWriter.hpp"
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <vector>

// --------------------------------------------------
// MAIN WRITER
// --------------------------------------------------
void VTKWriter::writeVTU(
    const MeshBase& mesh,
    const std::vector<double>& field,
    const std::filesystem::path& filename)
{
    std::ofstream f(filename);

    if (!f.is_open())
        throw std::runtime_error("Failed to open VTU file");

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

    connectivity.reserve(Ncells * 8); // safe upper bound
    offsets.reserve(Ncells);
    types.reserve(Ncells);

    std::size_t runningOffset = 0;

    for (std::size_t c = 0; c < Ncells; ++c)
    {
        std::vector<std::size_t> nodes;
        mesh.cellNodes(c, nodes);

        int vtkType = mesh.vtkCellType(c);

        for (auto n : nodes)
            connectivity.push_back(n);

        runningOffset += nodes.size();
        offsets.push_back(runningOffset);
        types.push_back(vtkType);
    }

    // -------------------------
    // Connectivity
    // -------------------------
    f << "<Cells>\n";

    f << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";
    for (auto n : connectivity)
        f << n << " ";
    f << "\n</DataArray>\n";

    // -------------------------
    // Offsets
    // -------------------------
    f << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
    for (auto o : offsets)
        f << o << "\n";
    f << "</DataArray>\n";

    // -------------------------
    // Types
    // -------------------------
    f << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
    for (auto t : types)
        f << t << "\n";
    f << "</DataArray>\n";

    f << "</Cells>\n";

    // =========================================================
    // CELL DATA
    // =========================================================
    f << "<CellData Scalars=\"field\">\n";
    f << "<DataArray type=\"Float64\" Name=\"field\" format=\"ascii\">\n";

    for (std::size_t c = 0; c < Ncells; ++c)
        f << field[c] << "\n";

    f << "</DataArray>\n";
    f << "</CellData>\n";

    f << "</Piece>\n";
    f << "</UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}
