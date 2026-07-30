#pragma once
#include "config/PathContext.hpp"
#include "config/SimulationConfig.hpp"

PathContext buildPaths(const SimulationConfig& cfg, const std::filesystem::path& projectRoot);
