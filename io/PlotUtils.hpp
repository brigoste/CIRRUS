#pragma once

#include "config/PathContext.hpp"

#include <filesystem>

void runPlot( const PathContext& paths, const std::filesystem::path& csvFile );
