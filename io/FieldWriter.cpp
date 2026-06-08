#include "io/FieldWriter.hpp"
#include <fstream>
#include <stdexcept>

void FieldWriter::writeCSVDebug(
    const PointField& field,
    const std::vector<double>& rhs,
    const std::vector<double>& residual,
    const std::string& filename)
{
    std::ofstream f(filename);

    if (!f.is_open())
        throw std::runtime_error("Failed to open debug CSV");

    f << "x,y,z,phi,rhs,residual\n";

    const std::size_t N = field.phi.size();

    for (std::size_t i = 0; i < N; ++i)
    {
        const Point& p = field.x[i];

        const double rhs_i =
            (i < rhs.size()) ? rhs[i] : 0.0;

        const double res_i =
            (i < residual.size()) ? residual[i] : 0.0;

        f << p.x[0] << ","
          << p.x[1] << ","
          << p.x[2] << ","
          << field.phi[i] << ","
          << rhs_i << ","
          << res_i << "\n";
    }
}
