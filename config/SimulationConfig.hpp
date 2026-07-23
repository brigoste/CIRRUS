#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "Solver/SolverMethod.hpp"
#include "config/PhysicsType.hpp"
#include "mesh/BoundaryPatchSystem.hpp"

//==================================================
// Boundary
//==================================================

struct BoundaryConfig
{
    std::size_t group = 0;
    BoundaryPatchSystem::Condition condition{};
};

//==================================================
// Mesh
//==================================================

struct MeshConfig
{
    std::string type = "line1D";

    std::size_t nx = 1;
    std::size_t ny = 1;

    double lx = 1.0;
    double ly = 1.0;
};

//==================================================
// Physics
//==================================================

struct PhysicsConfig
{
    physics::PhysicsType type = physics::PhysicsType::Heat;

    double k     = 1.0;
    double gamma = 0.0;

    double rho = 1.0;

    double ux = 0.0;
    double uy = 0.0;
    double uz = 0.0;
};

//==================================================
// Solver
//==================================================

struct SolverConfig
{
    solver::Method method = solver::Method::TDMA;

    int max_iter = 1000;
    double tol   = 1e-10;
    double omega = 1.0;
};

//==================================================
// I/O
//==================================================

struct IOConfig
{
    std::string output_root = "output";
    bool plot_enabled = true;
};

//==================================================
// Verification
//==================================================

struct RefinementConfig
{
    bool enabled = false;

    std::vector<int> levels;

    double expected_order = 2.0;
};

struct VerificationCaseEntry
{
    std::string name;

    MeshConfig mesh;
    PhysicsConfig physics;
    SolverConfig solver;
    RefinementConfig refinement;

    std::vector<BoundaryConfig> boundary;

    bool overrideMesh = false;
    bool overridePhysics = false;
    bool overrideSolver = false;
    bool overrideBoundary = false;
    bool overrideRefinement = false;

    nlohmann::json params;
};

struct VerificationSuite
{
    bool enabled = false;
    bool plot_enabled = true;

    std::vector<VerificationCaseEntry> cases;

    struct Output
    {
        std::string directory = "output/verification";
    } output;
};



//==================================================
// Top-level simulation configuration
//==================================================

struct SimulationConfig
{
    std::string extends;

    MeshConfig mesh;
    PhysicsConfig physics;
    SolverConfig solver;
    IOConfig io;

    std::vector<BoundaryConfig> boundary;

    VerificationSuite verificationSuite;
};

SimulationConfig resolveCaseConfig( const SimulationConfig& base, const VerificationCaseEntry& entry);

//==================================================
// Factory functions
//==================================================

SimulationConfig defaultConfig();

SimulationConfig loadConfig(const std::filesystem::path& path);

nlohmann::json mergeJson( nlohmann::json base, const nlohmann::json& override_);

SimulationConfig fromJson(const nlohmann::json& j);

// ------------------------- Physics --------------------------
inline void from_json(const nlohmann::json& j, PhysicsConfig& p)
{
    p.k     = j.value("k", 1.0);
    p.gamma = j.value("gamma", 0.0);
    p.rho   = j.value("rho", 1.0);
    p.ux    = j.value("ux", 0.0);
    p.uy    = j.value("uy", 0.0);
    p.uz    = j.value("uz", 0.0);

    if (j.contains("type")) { p.type = physics::physicsFromString(j.at("type").get<std::string>());}
}

// ----------------- Boundary --------------------------
inline void from_json(const nlohmann::json& j, BoundaryConfig& b)
{
    b.group = j.value("group", 0);

    auto type = bc::from_string(j.value("type", "Dirichlet"));

    b.condition.type = type;

    b.condition.value = 0.0;
    b.condition.flux  = 0.0;
    b.condition.h     = 0.0;
    b.condition.Tinf  = 0.0;

    switch (type)
    {
        case bc::Type::Dirichlet:
            b.condition.value = j.value("value", 0.0);
            break;

        case bc::Type::Neumann:
            b.condition.flux = j.value("flux", 0.0);
            break;

        case bc::Type::Convective:
            b.condition.h    = j.value("h", 0.0);
            b.condition.Tinf = j.value("Tinf", 0.0);
            break;

        default:
            break;
    }
}

// -------------- SolverConfig --------------------
inline void from_json(const nlohmann::json& j, SolverConfig& s)
{
    s.method   = j.value("type", solver::Method::TDMA);
    s.tol      = j.value("tol", 1e-10);
    s.max_iter = j.value("max_iter", 1000);
    s.omega    = j.value("omega", 1.0);
}

// --------------------- MESH -----------------------------
inline void from_json(const nlohmann::json& j, MeshConfig& m)
{
    m.type = j.value("type", "line1D");
    m.nx   = j.value("nx", 1);
    m.ny   = j.value("ny", 1);
    m.lx   = j.value("lx", 1.0);
    m.ly   = j.value("ly", 1.0);
}

// --------------------- REFINEMENT -----------------------------
inline void from_json(const nlohmann::json& j, RefinementConfig& r)
{
    r.enabled = j.value("enabled", false);

    if (j.contains("levels")) { r.levels = j.at("levels").get<std::vector<int>>(); }

    r.expected_order = j.value("expected_order", 2.0);
}
