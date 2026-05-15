#include "io/FieldWriter.hpp"
#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include <fstream>
#include <stdexcept>

void FieldWriter::writeCSVDebug(
    const MeshBase& mesh,
    const std::vector<double>& phi,
    const std::vector<double>& rhs,
    const std::vector<double>& residual,
    const std::string& filename)
{
    std::ofstream f(filename);

    if (!f.is_open())
        throw std::runtime_error("Failed to open debug CSV");

    const std::size_t N = mesh.ncells();

    Point p0 = mesh.cellCenter(0);

    if (p0.x[1] == 0.0 && p0.x[2] == 0.0)
    {
        f << "x,phi,rhs,residual\n";

        for (std::size_t i = 0; i < N; ++i)
        {
            Point p = mesh.cellCenter(i);

            f << p.x[0] << ","
              << phi[i] << ","
              << rhs[i] << ","
              << residual[i] << "\n";
        }
    }
    else
    {
        f << "x,y,phi,rhs,residual\n";

        for (std::size_t i = 0; i < N; ++i)
        {
            Point p = mesh.cellCenter(i);

            f << p.x[0] << ","
              << p.x[1] << ","
              << phi[i] << ","
              << rhs[i] << ","
              << residual[i] << "\n";
        }
    }
}
