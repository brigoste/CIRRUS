#include "io/FieldWriter.hpp"
#include <fstream>
#include <stdexcept>
#include <iostream>
#include <filesystem>

void FieldWriter::writeCSVDebug(
    const PointField& field,
    const std::vector<double>& rhs,
    const std::vector<double>& residual,
    const std::string& filename)
{
    std::filesystem::path path(filename);

    // -------------------------
    // Only create directories if there is a parent path
    // -------------------------
    if (path.has_parent_path()) { std::filesystem::create_directories(path.parent_path()); }

    std::ofstream f(path);

    if (!f.is_open()) { throw std::runtime_error("Failed to open debug CSV: " + path.string()); }

    f << "x,y,z,phi,rhs,residual\n";

    const std::size_t N = field.phi.size();

    for (std::size_t i = 0; i < N; ++i)
    {
        const Point& p = field.x[i];

        const double rhs_i = (i < rhs.size()) ? rhs[i] : 0.0;

        const double res_i = (i < residual.size()) ? residual[i] : 0.0;

        f << p.x[0] << ","
          << p.x[1] << ","
          << p.x[2] << ","
          << field.phi[i] << ","
          << rhs_i << ","
          << res_i << "\n";
    }
}
