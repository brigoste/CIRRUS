#include "io/OutputManager.hpp"

#include "io/OutputData.hpp"

#include "io/CSVWriter.hpp"
#include "io/VTKWriter.hpp"
#include "io/MetadataWriter.hpp"

void OutputManager::write(
    const OutputData& data,
    const std::filesystem::path& outputDirectory)
{
    CSVWriter::write( data, outputDirectory / "solution.csv");

    VTKWriter::write( data, outputDirectory / "solution.vtu");

    MetadataWriter::write( data, outputDirectory / "metadata.json");
}
