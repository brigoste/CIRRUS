#pragma once

#include <filesystem>

struct OutputData;

class MetadataWriter
{
public:

    static void write( const OutputData& data, const std::filesystem::path& filename);
};
