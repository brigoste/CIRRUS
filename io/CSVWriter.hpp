#pragma once

#include <filesystem>

struct OutputData;

class CSVWriter
{
public:

    static void write( const OutputData& data, const std::filesystem::path& filename);
};
