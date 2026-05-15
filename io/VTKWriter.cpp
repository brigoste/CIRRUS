#include "io/VTKWriter.hpp"
#include <fstream>
#include <stdexcept>
#include <cmath>
#include "mesh/PointsUtils.hpp"

// static void ensureOpen(const std::ofstream& f, const std::string& path)
// {
//     if (!f.is_open())
//         throw std::runtime_error("Failed to open VTU file: " + path);
// }

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

    // =========================================================
    // POINTS
    // =========================================================
    f << "<Piece NumberOfPoints=\"" << Nnodes
      << "\" NumberOfCells=\"" << Ncells << "\">\n";

    f << "<Points>\n";
    f << "<DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";

    for (std::size_t i = 0; i < Nnodes; ++i)
    {
        Point p = mesh.node(i);

        f << p.x[0] << " "
          << p.x[1] << " "
          << p.x[2] << "\n";
    }

    f << "</DataArray>\n";
    f << "</Points>\n";

    // =========================================================
    // CELLS
    // =========================================================
    f << "<Cells>\n";

    f << "<DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";

    int nodesPerCell = mesh.cellNodeCount(0);

    for (std::size_t c = 0; c < Ncells; ++c)
    {
        for (int k = 0; k < nodesPerCell; ++k)
        {
            f << mesh.cellNode(c, k) << " ";
        }
        f << "\n";
    }

    f << "</DataArray>\n";

    f << "<DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
    for (std::size_t c = 0; c < Ncells; ++c)
        f << (c + 1) * nodesPerCell << "\n";
    f << "</DataArray>\n";

    f << "<DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";

    int vtkType =
        (nodesPerCell == 2) ? 3 :   // line
        (nodesPerCell == 4) ? 9 :   // quad
        7;                          // polygon fallback

    for (std::size_t c = 0; c < Ncells; ++c)
        f << vtkType << "\n";

    f << "</DataArray>\n";

    f << "</Cells>\n";

    // =========================================================
    // CELL DATA
    // =========================================================
    f << "<CellData Scalars=\"field\">\n";
    f << "<DataArray type=\"Float64\" Name=\"field\" format=\"ascii\">\n";

    for (std::size_t c = 0; c < Ncells; ++c)
    {
        f << field[c] << "\n";
    }

    f << "</DataArray>\n";
    f << "</CellData>\n";

    f << "</Piece>\n";
    f << "</UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}
