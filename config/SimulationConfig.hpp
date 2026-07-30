#pragma once

#include <filesystem>
#include <string>
#include <vector>

#include <nlohmann/json.hpp>

#include "solver/SolverMethod.hpp"
#include "config/PhysicsType.hpp"
#include "mesh/BoundaryPatchSystem.hpp"

#include <unordered_map>

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

    double k                = 1.0;
    double gamma            = 0.0;

    double rho              = 1.0;

    double ux               = 0.0;
    double uy               = 0.0;
    double uz               = 0.0;
    
    double volumetricSource = 0.0;
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

struct VerificationCaseConfig
{
    std::string name;
    std::string type;

    bool l2 = true;
    bool linf = true;
    bool plot_enabled = true;

    RefinementConfig refinement;

    bool overrideMesh = false;
    MeshConfig mesh;

    bool overridePhysics = false;
    PhysicsConfig physics;

    bool overrideSolver = false;
    SolverConfig solver;

    bool overrideBoundary = false;
    std::vector<BoundaryConfig> boundary;

    bool overrideRefinement = false;

    nlohmann::json params;
};

struct VerificationSuite
{
    bool enabled = false;
    bool plot_enabled = true;

    std::vector<VerificationCaseConfig> cases;

    std::unordered_map<std::string, VerificationCaseConfig> caseConfigs;

    std::string case_directory = "cases/verification";

    struct Output
    {
        std::string directory = "verification";
    } output;
};

struct SimulationConfig
{
    std::string extends;

    MeshConfig mesh;
    PhysicsConfig physics;
    SolverConfig solver;
    IOConfig io;

    std::vector<BoundaryConfig> boundary;

    VerificationSuite verificationSuite;

    VerificationCaseConfig verification;
};

//==================================================
// Factory functions
//==================================================

SimulationConfig defaultConfig();

SimulationConfig loadConfig(const std::filesystem::path& path);

SimulationConfig resolveCaseConfig( const SimulationConfig& cfg, const VerificationCaseConfig& entry);

nlohmann::json mergeJson( nlohmann::json base, const nlohmann::json& override_);

VerificationSuite loadVerificationSuite( const std::filesystem::path& path);

SimulationConfig fromJson(const nlohmann::json& j);

// ------------------------- Physics --------------------------
inline void from_json(const nlohmann::json& j, PhysicsConfig& p)
{
    p.k      = j.value("k", 1.0);
    p.gamma  = j.value("gamma", 0.0);
    p.rho    = j.value("rho", 1.0);
    p.ux     = j.value("ux", 0.0);
    p.uy     = j.value("uy", 0.0);
    p.uz     = j.value("uz", 0.0);
    p.volumetricSource = j.value("volumetricSource", 0.0);

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

// ------------------------- Read Config for Verification ------------
inline void from_json(const nlohmann::json& j, VerificationCaseConfig& v)
{
    v.name = j.value("name", "");
    v.type = j.value("type", "");

    v.l2 = j.value("l2", true);
    v.linf = j.value("linf", true);
    v.plot_enabled = j.value("plot_enabled", true);

    if (j.contains("refinement")) { v.refinement = j.at("refinement").get<RefinementConfig>(); }

    if (j.contains("mesh"))
    {
        v.mesh = j.at("mesh").get<MeshConfig>();
        v.overrideMesh = true;
    }

    if (j.contains("physics"))
    {
        v.physics = j.at("physics").get<PhysicsConfig>();
        v.overridePhysics = true;
    }

    if (j.contains("solver"))
    {
        v.solver = j.at("solver").get<SolverConfig>();
        v.overrideSolver = true;
    }

    if (j.contains("boundary_conditions"))
    {
        v.boundary = j.at("boundary_conditions").get<std::vector<BoundaryConfig>>();
        v.overrideBoundary = true;
    }

    v.params = j.value("params", nlohmann::json::object());
}

// -------------------- Read Verification Suite -------------------
inline void from_json(const nlohmann::json& j, VerificationSuite& v)
{
    v.enabled = j.value("enabled", false);
    v.plot_enabled = j.value("plot_enabled", true);

    v.cases.clear();

    if (j.contains("cases"))
    {
        for (const auto& c : j.at("cases"))
        {
            VerificationCaseConfig entry;
            entry.name = c.get<std::string>();
            v.cases.push_back(entry);
        }
    }
    if (j.contains("caseConfigs"))
    {
        for (auto& [name, cfg] : j["caseConfigs"].items())
        {
            VerificationCaseConfig entry =
                cfg.get<VerificationCaseConfig>();

            entry.name = name;

            v.caseConfigs[name] = entry;
        }
    }

    v.case_directory = j.value(
        "case_directory",
        "cases/verification"
    );

    if (j.contains("output"))
    {
        v.output.directory =
            j.at("output").value("directory", "verification");
    }
}

inline void from_json( const nlohmann::json& j, SimulationConfig& cfg)
{
    if (j.contains("extends")) { cfg.extends = j.at("extends").get<std::string>(); }
    if (j.contains("mesh")) { cfg.mesh = j.at("mesh") .get<MeshConfig>(); }
    if (j.contains("physics")) { cfg.physics = j.at("physics") .get<PhysicsConfig>(); }
    if (j.contains("solver")) { cfg.solver = j.at("solver") .get<SolverConfig>(); }
    if (j.contains("paths"))
    {
        cfg.io.output_root = j.at("paths").value("output_root", "output");
        cfg.io.plot_enabled = j.at("paths").value("plot_enabled", true);
    }
    if (j.contains("boundary_conditions")) { cfg.boundary = j.at("boundary_conditions").get<std::vector<BoundaryConfig>>(); }
    // if (j.contains("verificationSuite")) { cfg.verificationSuite = j.at("verificationSuite").get<VerificationSuite>(); }
    if (j.contains("verificationCase")) { cfg.verification = j.at("verificationCase").get<VerificationCaseConfig>(); }
}
