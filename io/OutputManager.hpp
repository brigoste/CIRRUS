#pragma once

#include <filesystem>

struct OutputData;

class OutputManager
{
public:

    static void write( const OutputData& output, const std::filesystem::path& outputDirectory);
};
