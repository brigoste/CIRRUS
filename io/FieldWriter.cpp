#include "io/FieldWriter.hpp"
#include <fstream>
#include <iostream>

void FieldWriter::writeCSV(
    const MeshBase& mesh,
    const std::vector<double>& field,
    const std::string& filename)
{
    std::ofstream f(filename);
    std::cout << "Writing data to " << filename << std::endl;

    if (!f.is_open())
    {
        std::cerr << "FAILED to open file: " << filename << std::endl;
        return;
    }

    f << "x,y,z,value\n";

    for (int i = 0; i < mesh.size(); i++)
    {
        auto p = mesh.point(i);

        double x = (p.x.size() > 0) ? p.x[0] : 0.0;
        double y = (p.x.size() > 1) ? p.x[1] : 0.0;
        double z = (p.x.size() > 2) ? p.x[2] : 0.0;

        f << x << "," << y << "," << z << "," << field[i] << "\n";
        // std::cout << x << "," << y << "," << z << "," << field[i] << "\n";
    }
}
