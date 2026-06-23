#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <functional>
#include <iostream>
#include <stdexcept>
#include <fstream>
#include <unordered_set>

#include "Solver/SolverMethod.hpp"
#include "mesh/primitives/Point.hpp"
#include "mesh/BoundaryPatchSystem.hpp"

#include <nlohmann/json.hpp>
// -----------------------------
// Forward-safe definitions
// -----------------------------

struct BoundaryConfig
{
    std::size_t group = 0;
    BoundaryPatchSystem::Condition condition{};
};

struct MeshConfig
{
    std::string type = "line1d"; // "line1d", "quad2D"

    std::size_t nx = 0; // Needed for all dimensions (strucutred)
    std::size_t ny = 1; // only needed for 2+ dimensions (structred)

    double lx = 0.0;    // Needed for all dimensions (structured)
    double ly = 1.0;    // only needed for 2+ dimensions (structured)
};

struct PhysicsConfig
{
    std::string type;   // "heat", "navier-stokes", etc.

    double k = 0.0;

    double Su = 0.0;
    double Sp = 0.0;
};

struct VerificationSuite
{
    bool enabled = false;
    bool plot_enabled = false;

    std::vector<std::string> cases;

    struct Output
    {
        std::string directory = "output/verification";
    } output;
};

struct SimulationConfig
{
    std::string extends;

    MeshConfig mesh;
    PhysicsConfig physics;

    std::vector<BoundaryConfig> boundary;

    struct SolverConfig
    {
        solver::Method method;
        int max_iter;
        double tol;
        double omega;
    } solver;

    VerificationSuite verificationSuite;    // ✅ HERE

    struct IOConfig {
        std::string output_root;
        bool plot_enabled = false;
    } io;

};


// using json = nlohmann::json;

// factories
SimulationConfig defaultConfig();

SimulationConfig loadConfig(const std::filesystem::path& path);

nlohmann::json mergeJson(
    nlohmann::json base,
    const nlohmann::json& override);

SimulationConfig fromJson(const nlohmann::json& j);
