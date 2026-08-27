#include "io/CSVWriter.hpp"

#include "io/OutputData.hpp"
#include "io/PointField.hpp"

#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <iomanip>

void CSVWriter::write(
    const OutputData& data,
    const std::filesystem::path& filename)
{
    if (filename.has_parent_path()) { std::filesystem::create_directories(filename.parent_path()); }

    std::ofstream f(filename);

    if (!f.is_open()) { throw std::runtime_error("Failed to open CSV output: " + filename.string()); }
    
    f << std::setprecision(16);
    f << "x,y,z,phi,rhs,residual\n";

    const auto& field = *data.reconstructedField;

    if (!data.reconstructedField) { throw std::runtime_error("CSVWriter requires reconstructed point field"); }

    const std::size_t N = field.phi.size();

    for (std::size_t i = 0; i < N; ++i)
    {
        const Point& p = field.x[i];

        const double rhs_i = (i < data.rhs.size()) ? data.rhs[i] : 0.0;

        const double res_i = (i < data.residual.size()) ? data.residual[i] : 0.0;

        f << p.x[0] << ","
          << p.x[1] << ","
          << p.x[2] << ","
          << field.phi[i] << ","
          << rhs_i << ","
          << res_i << "\n";
    }
}
