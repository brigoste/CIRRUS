#include "config/ConfigResolver.hpp"
#include "config/SimulationConfig.hpp"

SimulationConfig ConfigResolver::load(
    const std::filesystem::path& path)
{
    nlohmann::json resolved = loadAndResolve(path);

    SimulationConfig cfg = defaultConfig();

    // build cfg from resolved JSON here

    return cfg;
}

nlohmann::json ConfigResolver::loadAndResolve(
    const std::filesystem::path& path)
{
    auto absPath =
        std::filesystem::absolute(path);

    // -------------------------
    // LOAD FILE
    // -------------------------

    nlohmann::json j =
        resolveFile(absPath);

    // -------------------------
    // HANDLE EXTENDS
    // -------------------------

    nlohmann::json base;

    if (j.contains("extends"))
    {
        std::vector<std::string> parents;

        if (j["extends"].is_string()) { parents.push_back( j["extends"].get<std::string>()); }
        else if (j["extends"].is_array()) { parents = j["extends"].get<std::vector<std::string>>(); }
        else { throw std::runtime_error("'extends' must be string or array"); }

        for (const auto& rel : parents)
        {
            auto parentPath = std::filesystem::weakly_canonical(absPath.parent_path() / rel);

            nlohmann::json parentJson = loadAndResolve(parentPath);

            base = mergeJson(base, parentJson);
        }

        // inheritance is resolved now
        j.erase("extends");
    }

    // -------------------------
    // FINAL MERGE
    // -------------------------

    return mergeJson(base, j);
}

nlohmann::json ConfigResolver::resolveFile(
    const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { throw std::runtime_error("Cannot open config: "+ path.string()); }

    nlohmann::json j;
    file >> j;

    return j;
}
