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
#include "config/PhysicsType.hpp"

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
    physics::PhysicsType type;

    double k = 0.0;
    double gamma = 0.0;

    double rho = 1.0;
    double ux = 0.0, uy = 0.0, uz = 0.0;
};


struct VerificationParams
{
    double k = 0.0;
    double gamma = 0.0;
    double ux = 0.0;
    double uy = 0.0;
    double uz = 0.0;
};

struct VerificationCaseEntry
{
    std::string name;
    nlohmann::json params;
};

struct VerificationSuite
{
    bool enabled = true;
    bool plot_enabled = false;

    std::vector<VerificationCaseEntry> cases;

    struct Output
    {
        std::string directory = "output/verification";
    } output;
};

struct SimulationConfig
{
    std::string extends;

    MeshConfig mesh;
    PhysicsConfig physics;   // ✅ FIXED

    std::vector<BoundaryConfig> boundary;

    struct SolverConfig
    {
        solver::Method method;
        int max_iter;
        double tol;
        double omega;
    } solver;

    VerificationSuite verificationSuite;

    struct IOConfig
    {
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
