#include "config/SimulationConfig.hpp"
#include <unordered_set>
#include <fstream>
#include <iostream>
// ============================================================
// JSON merge (recursive override)
// ============================================================

const std::unordered_set<std::string> atomicKeys = {
    "extends",
    "boundary_conditions",
    "mesh",
    "solver",
    "physics",
    "paths"
};

nlohmann::json mergeJson(nlohmann::json base, const nlohmann::json& override)
{
    for (auto& [key, value] : override.items())
    {
        if (atomicKeys.find(key) != atomicKeys.end())
        {
            base[key] = value;
            continue;
        }

        if (base.contains(key) && base[key].is_object() && value.is_object()) { base[key] = mergeJson(base[key], value); }
        else { base[key] = value; }
    }

    return base;
}

VerificationSuite loadVerificationSuite(const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { throw std::runtime_error( "Cannot open verification suite file: " + path.string()); }

    nlohmann::json j;
    file >> j;

    if (j.contains("extends"))
    {
        std::filesystem::path parent = std::filesystem::weakly_canonical( path.parent_path() / j.at("extends").get<std::string>());

        std::ifstream baseFile(parent);

        nlohmann::json baseJson;
        baseFile >> baseJson;

        j = mergeJson(baseJson, j);
    }

    if (!j.contains("verificationSuite")) { throw std::runtime_error( "Verification suite missing 'verificationSuite'"); }

    return j.at("verificationSuite").get<VerificationSuite>();
}

VerificationCaseConfig loadVerificationCase(const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { throw std::runtime_error( "Cannot open verification case: " + path.string()); }

    nlohmann::json j;
    file >> j;

    VerificationCaseConfig cfg;

    cfg.name = path.stem().string();

    if (j.contains("mesh")) { cfg.mesh = j.at("mesh").get<MeshConfig>(); }
    if (j.contains("physics")) { cfg.physics = j.at("physics").get<PhysicsConfig>(); }
    if (j.contains("solver")) { cfg.solver = j.at("solver").get<SolverConfig>(); }
    if (j.contains("boundary_conditions")) { cfg.boundary = j.at("boundary_conditions") .get<std::vector<BoundaryConfig>>(); }
    if (j.contains("refinement")) { cfg.refinement = j.at("refinement").get<RefinementConfig>(); }
    if (j.contains("discretization")) { cfg.discretization = j.at("discretization").get<DiscretizationConfig>(); }
    if (j.contains("params")) { cfg.params = j.at("params"); }

    return cfg;
}

// ============================================================
// Defaults
// ============================================================

SimulationConfig defaultConfig()
{
    SimulationConfig cfg;

    cfg.mesh.type = "line1D";
    cfg.mesh.nx = 50;
    cfg.mesh.ny = 1;
    cfg.mesh.lx = 1.0;
    cfg.mesh.ly = 1.0;

    cfg.physics.type = physics::PhysicsType::Heat;
    cfg.physics.k = 1.0;

    cfg.solver.method = solver::Method::SOR;
    cfg.solver.tol = 1e-8;
    cfg.solver.max_iter = 5000;
    cfg.solver.omega = 1.0;
    cfg.solver.restart = 10;
    cfg.solver.preconditioner = PreconditionerType::None;

    cfg.io.output_root = "output";
    cfg.io.plot_enabled = false;

    cfg.discretization.gradientScheme = GradientType::GreenGauss;
    cfg.discretization.reconstructionScheme = ReconstructionType::Gradient;

    cfg.boundary.clear();

    return cfg;
}

SimulationConfig loadConfig( const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open()) { throw std::runtime_error( "Cannot open config file: " + path.string()); }

    nlohmann::json j;
    file >> j;

    // -------------------------------------------------
    // Handle inheritance (extends)
    // -------------------------------------------------
    if (j.contains("extends")) {
        std::vector<std::string> parents;

        if (j.at("extends").is_string()) { parents.push_back(j.at("extends").get<std::string>()); }
        else if (j.at("extends").is_array()) { parents = j.at("extends").get<std::vector<std::string>>(); }
        else { throw std::runtime_error("`extends` must be string or array of strings"); }

        nlohmann::json base;

        for (const auto& parentRel : parents)
        {
            std::filesystem::path parentPath = std::filesystem::weakly_canonical(path.parent_path() / parentRel);

            if (!std::filesystem::exists(parentPath)) { throw std::runtime_error("Cannot open parent config: " + parentPath.string()); }

            std::ifstream baseFile(parentPath);

            if (!baseFile.is_open()) { throw std::runtime_error("Cannot open parent config: " + parentPath.string()); }

            nlohmann::json parentJson;
            baseFile >> parentJson;

            base = mergeJson(base, parentJson);
        }

        // finally merge child on top of all parents
        j = mergeJson(base, j);
    }

    // -------------------------------------------------
    // Validate minimal structure
    // -------------------------------------------------
    if (!j.contains("mesh")) { throw std::runtime_error("Config missing 'mesh'"); }
    if (!j.contains("physics")) { throw std::runtime_error("Config missing 'physics'"); }

    // -------------------------------------------------
    // Build config from defaults
    // -------------------------------------------------
    // std::cout << j.dump(2) << "\n";

    return fromJson(j);
}

SimulationConfig fromJson(const nlohmann::json& j)
{
    SimulationConfig cfg = defaultConfig();

    // -------------------------------------------------
    // Mesh
    // -------------------------------------------------
    cfg.mesh.type = j.at("mesh").at("type").get<std::string>();
    cfg.mesh.nx   = j.at("mesh").value("nx", 1);
    cfg.mesh.ny   = j.at("mesh").value("ny", 1);
    cfg.mesh.lx   = j.at("mesh").value("lx", 1.0);
    cfg.mesh.ly   = j.at("mesh").value("ly", 1.0);

    std::cout << "Requested mesh = " << cfg.mesh.nx << " x " << cfg.mesh.ny << '\n';

    // -------------------------------------------------
    // Physics
    // -------------------------------------------------
    cfg.physics.type    = physics::physicsFromString(j.at("physics").at("type").get<std::string>());

    cfg.physics.k       = j.at("physics").value("transferCoefficient", 0.0);
    cfg.physics.gamma   = j.at("physics").value("gamma", 0.0);

    cfg.physics.rho     = j.at("physics").value("rho", 1.0);
    cfg.physics.ux      = j.at("physics").value("ux", 0.0);
    cfg.physics.uy      = j.at("physics").value("uy", 0.0);
    cfg.physics.uz      = j.at("physics").value("uz", 0.0);
    // -------------------------------------------------
    // Solver
    // -------------------------------------------------
    cfg.solver          = j.at("solver").get<SolverConfig>();
    // -------------------------------------------------
    // Output paths (normalized)
    // -------------------------------------------------
    if (j.contains("paths"))
    {
        const auto& paths = j.at("paths");

        cfg.io.output_root = paths.value("output_root", cfg.io.output_root);
        cfg.io.python_executable = paths.value("python_executable", "python");
        cfg.io.plot_enabled = paths.value("plot_enabled", true);
    }

    // -------------------------------------------------
    // Boundary conditions
    // -------------------------------------------------
    cfg.boundary.clear();

    if (!j.contains("boundary_conditions")) { throw std::runtime_error("Config missing 'boundary_conditions'"); }

    for (const auto& bcJson : j.at("boundary_conditions"))
    {        
        BoundaryConfig bc{};

        bc.group = bcJson.at("group").get<std::size_t>();

        bc.condition.type  = bc::from_string(bcJson.at("type").get<std::string>());

        bc.condition.value = 0.0;
        bc.condition.flux  = 0.0;
        bc.condition.transferCoefficient     = 0.0;
        bc.condition.referenceValue  = 0.0;

        switch (bc.condition.type)
        {
            case bc::Type::Dirichlet:
                if (!bcJson.contains("value"))
                {
                    throw std::runtime_error(
                        "Dirichlet boundary condition requires 'value'");
                }
                bc.condition.value = bcJson.at("value").get<double>();
                break;

            case bc::Type::Neumann:
                if (!bcJson.contains("flux"))
                {
                    throw std::runtime_error(
                        "Neumann boundary condition requires 'flux'");
                }
                bc.condition.flux = bcJson.at("flux").get<double>();
                break;

            case bc::Type::Robin:
                bc.condition.transferCoefficient = bcJson.at("transferCoefficient").get<double>();
                bc.condition.referenceValue = bcJson.at("referenceValue").get<double>();
                break;

            default:
                break;
        }

        cfg.boundary.push_back(bc);
    }

    // -------------------------------------------------
    // Verification (safe parsing)
    // -------------------------------------------------
   
    if (j.contains("verificationSuite") && j["verificationSuite"].contains("caseConfigs"))
    {
        for (auto& [k, v] : j["verificationSuite"]["caseConfigs"].items()) { std::cout << "  " << k << "\n";}
    }
    else { std::cout << "NO CASE CONFIGS FOUND\n"; }

    return cfg;
}
