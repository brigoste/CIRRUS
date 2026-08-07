#include "OutputManager.hpp"

#include "OutputData.hpp"

#include "CSVWriter.hpp"
#include "VTKWriter.hpp"
// #include "JSONWriter.hpp"

void OutputManager::write(
    const OutputData& data,
    const std::filesystem::path& outputDirectory)
{
    CSVWriter::write(
        data,
        outputDirectory / "solution.csv");

    VTKWriter::write(
        data,
        outputDirectory / "solution.vtu");

    // JSONWriter::write(
    //     data,
    //     outputDirectory);
}