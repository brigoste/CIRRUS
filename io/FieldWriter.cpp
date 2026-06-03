#include "io/FieldWriter.hpp"
#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include <fstream>
#include <stdexcept>
#include <cmath>

void FieldWriter::writeCSVDebug(
    const Field1D& field,
    const std::vector<double>& rhs,
    const std::vector<double>& residual,
    const std::string& filename)
{
    std::ofstream f(filename);

    if (!f.is_open())
        throw std::runtime_error("Failed to open debug CSV");

    f << "x,phi,rhs,residual\n";

    const std::size_t N = field.phi.size();

    for (std::size_t i = 0; i < N; ++i)
    {
        if (i == 0 || i == N-1) {
            f << field.x[i] << "," << field.phi[i] << "," << 0 << "," << residual[i] << "\n";
        }
        else {
            f << field.x[i] << "," << field.phi[i] << "," << rhs[i-1] << "," << residual[i] << "\n";
        }
    }
}

// void FieldWriter::writeCSVDebug(
//     const Field1D& field,
//     const std::vector<double>& rhs,
//     const std::vector<double>& residual,
//     const std::string& filename)
// {
//     if (field.x.size() != field.phi.size())
//         throw std::runtime_error("Field1D size mismatch");

//     std::ofstream file(filename);

//     file << "x,phi,rhs,residual\n";

//     for (std::size_t i = 0; i < field.x.size(); ++i)
//     {
//         file << field.x[i] << ","
//              << field.phi[i] << ","
//              << (i < rhs.size() ? rhs[i] : 0.0) << ","
//              << (i < residual.size() ? residual[i] : 0.0)
//              << "\n";
//     }
// }
