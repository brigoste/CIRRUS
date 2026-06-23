#pragma once

#include <filesystem>
#include <string>

#include "config/SimulationConfig.hpp"

struct PathContext
{
    std::filesystem::path outputRoot;
    std::filesystem::path verificationRoot;
};
