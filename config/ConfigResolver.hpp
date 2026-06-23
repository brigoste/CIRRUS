#pragma once

#include <filesystem>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "config/SimulationConfig.hpp"

// struct SimulationConfig;

class ConfigResolver
{
public:
    ConfigResolver() = default;

    SimulationConfig load(const std::filesystem::path& path);

private:
    nlohmann::json loadAndResolve(
        const std::filesystem::path& path);

    nlohmann::json resolveFile(
        const std::filesystem::path& path);

private:
    std::unordered_map<std::filesystem::path, nlohmann::json> cache;
};
