#include "io/VTKWriter.hpp"
#include <fstream>
#include <stdexcept>
#include <cmath>

// static void ensureOpen(const std::ofstream& f, const std::string& path)
// {
//     if (!f.is_open())
//         throw std::runtime_error("Failed to open VTU file: " + path);
// }

// --------------------------------------------------
// Helper: detect dimension
// --------------------------------------------------
static int dim(const MeshBase& mesh)
{
    return mesh.dim();
}

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
        throw std::runtime_error("Failed to open VTU file: " + filename.string());

    const int N = mesh.size();

    // --------------------------------------------------
    // Header
    // --------------------------------------------------
    f << "<?xml version=\"1.0\"?>\n";
    f << "<VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\">\n";
    f << "  <UnstructuredGrid>\n";

    // ==================================================
    // 1D CASE: line cells
    // ==================================================
    if (dim(mesh) == 1)
    {
        int numCells = N - 1;

        f << "    <Piece NumberOfPoints=\"" << N
          << "\" NumberOfCells=\"" << numCells << "\">\n";

        // -------------------------
        // Points
        // -------------------------
        f << "      <Points>\n";
        f << "        <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";

        for (int i = 0; i < N; i++)
        {
            auto p = mesh.point(i);

            double x = p.x[0];
            f << x << " 0 0\n";
        }

        f << "        </DataArray>\n";
        f << "      </Points>\n";

        // -------------------------
        // Cells (connectivity)
        // -------------------------
        f << "      <Cells>\n";

        f << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";
        for (int i = 0; i < numCells; i++)
            f << i << " " << i + 1 << "\n";
        f << "        </DataArray>\n";

        f << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
        for (int i = 1; i <= numCells; i++)
            f << 2 * i << "\n";
        f << "        </DataArray>\n";

        f << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
        for (int i = 0; i < numCells; i++)
            f << 3 << "\n"; // VTK_LINE = 3
        f << "        </DataArray>\n";

        f << "      </Cells>\n";

        // -------------------------
        // Cell Data
        // -------------------------
        f << "      <CellData Scalars=\"field\">\n";
        f << "        <DataArray type=\"Float64\" Name=\"field\" format=\"ascii\">\n";

        for (int i = 0; i < numCells; i++)
        {
            double avg = 0.5 * (field[i] + field[i + 1]);
            f << avg << "\n";
        }

        f << "        </DataArray>\n";
        f << "      </CellData>\n";

        f << "    </Piece>\n";
    }

    // ==================================================
    // 2D STRUCTURED FVM (QUADS)
    // ==================================================
    else if (dim(mesh) == 2)
    {
        int nx = static_cast<int>(std::sqrt(N));
        int ny = nx;

        int numCells = (nx - 1) * (ny - 1);

        f << "    <Piece NumberOfPoints=\"" << N
          << "\" NumberOfCells=\"" << numCells << "\">\n";

        // -------------------------
        // Points
        // -------------------------
        f << "      <Points>\n";
        f << "        <DataArray type=\"Float64\" NumberOfComponents=\"3\" format=\"ascii\">\n";

        for (int j = 0; j < ny; j++)
        for (int i = 0; i < nx; i++)
        {
            int p = j * nx + i;
            auto pt = mesh.point(p);

            f << pt.x[0] << " " << pt.x[1] << " 0\n";
        }

        f << "        </DataArray>\n";
        f << "      </Points>\n";

        // -------------------------
        // Cells
        // -------------------------
        f << "      <Cells>\n";

        f << "        <DataArray type=\"Int32\" Name=\"connectivity\" format=\"ascii\">\n";

        for (int j = 0; j < ny - 1; j++)
        for (int i = 0; i < nx - 1; i++)
        {
            int p0 = j * nx + i;
            int p1 = p0 + 1;
            int p2 = p0 + nx + 1;
            int p3 = p0 + nx;

            f << p0 << " " << p1 << " " << p2 << " " << p3 << "\n";
        }

        f << "        </DataArray>\n";

        f << "        <DataArray type=\"Int32\" Name=\"offsets\" format=\"ascii\">\n";
        for (int i = 1; i <= numCells; i++)
            f << 4 * i << "\n";
        f << "        </DataArray>\n";

        f << "        <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">\n";
        for (int i = 0; i < numCells; i++)
            f << 9 << "\n"; // VTK_QUAD = 9
        f << "        </DataArray>\n";

        f << "      </Cells>\n";

        // -------------------------
        // Cell Data
        // -------------------------
        f << "      <CellData Scalars=\"field\">\n";
        f << "        <DataArray type=\"Float64\" Name=\"field\" format=\"ascii\">\n";

        for (int j = 0; j < ny - 1; j++)
        for (int i = 0; i < nx - 1; i++)
        {
            int p0 = j * nx + i;
            int p1 = p0 + 1;
            int p2 = p0 + nx;
            int p3 = p0 + nx + 1;

            double avg =
                0.25 * (field[p0] + field[p1] + field[p2] + field[p3]);

            f << avg << "\n";
        }

        f << "        </DataArray>\n";
        f << "      </CellData>\n";

        f << "    </Piece>\n";
    }

    else
    {
        throw std::runtime_error("VTKWriter: unsupported dimension");
    }

    // --------------------------------------------------
    // Footer
    // --------------------------------------------------
    f << "  </UnstructuredGrid>\n";
    f << "</VTKFile>\n";
}
