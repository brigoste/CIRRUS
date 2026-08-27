#pragma once

#include <filesystem>

class OutputData;

class VTKWriter
{
public:

    static void write( const OutputData& data, const std::filesystem::path& filename);

};
