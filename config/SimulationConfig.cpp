#include "config/SimulationConfig.hpp"
// ============================================================
// JSON merge (recursive override)
// ============================================================

namespace
{
    const std::unordered_set<std::string> atomicKeys = {
        "extends",
        "boundary_conditions",
        "verificationSuite"
    };
}

nlohmann::json mergeJson(nlohmann::json base, const nlohmann::json& override)
{
    for (auto& [key, value] : override.items())
    {
        if (atomicKeys.find(key) != atomicKeys.end())
        {
            base[key] = value;
            continue;
        }

        if (base.contains(key) &&
            base[key].is_object() &&
            value.is_object())
        {
            base[key] = mergeJson(base[key], value);
        }
        else
        {
            base[key] = value;
        }
    }

    return base;
}

// ============================================================
// Defaults
// ============================================================

SimulationConfig defaultConfig()
{
    SimulationConfig cfg;

    cfg.mesh.type = "line1D";
    cfg.mesh.nx = 50;
    cfg.mesh.lx = 1.0;

    cfg.physics.k = 1.0;

    cfg.solver.method = solver::Method::SOR;
    cfg.solver.tol = 1e-8;
    cfg.solver.max_iter = 5000;
    cfg.solver.omega = 1.0;

    cfg.verificationSuite.enabled = false;
    cfg.verificationSuite.cases.clear();
    cfg.verificationSuite.output.directory = "output/verification";

    return cfg;
}

SimulationConfig loadConfig(
    const std::filesystem::path& path)
{
    std::ifstream file(path);

    if (!file.is_open()){ throw std::runtime_error( "Cannot open config file: " + path.string()); }

    nlohmann::json j;
    file >> j;

    // -------------------------------------------------
    // Handle inheritance (extends)
    // -------------------------------------------------
    if (j.contains("extends")) {
        std::vector<std::string> parents;

        if (j.at("extends").is_string()) {
            parents.push_back(j.at("extends").get<std::string>());
        }
        else if (j.at("extends").is_array()) {
            parents = j.at("extends").get<std::vector<std::string>>();
        }
        else {
            throw std::runtime_error(
                "`extends` must be string or array of strings");
        }

        nlohmann::json base;

        for (const auto& parentRel : parents)
        {
            std::filesystem::path parentPath = std::filesystem::weakly_canonical(path.parent_path() / parentRel);

            if (!std::filesystem::exists(parentPath)) {
                throw std::runtime_error("Cannot open parent config: " + parentPath.string());
            }

            std::ifstream baseFile(parentPath);

            if (!baseFile.is_open()) {
                throw std::runtime_error("Cannot open parent config: " + parentPath.string());
            }

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
    if (!j.contains("physics")){ throw std::runtime_error("Config missing 'physics'"); }

    // -------------------------------------------------
    // Build config from defaults
    // -------------------------------------------------

    return fromJson(j);
}

SimulationConfig fromJson(
    const nlohmann::json& j)
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

    // -------------------------------------------------
    // Physics
    // -------------------------------------------------
    cfg.physics.type = physics::physicsFromString(j.at("physics").at("type").get<std::string>());

    cfg.physics.k     = j.at("physics").value("k", 0.0);
    cfg.physics.gamma = j.at("physics").value("gamma", 0.0);

    cfg.physics.rho = j.at("physics").value("rho", 1.0);
    cfg.physics.ux  = j.at("physics").value("ux", 0.0);
    cfg.physics.uy  = j.at("physics").value("uy", 0.0);
    cfg.physics.uz  = j.at("physics").value("uz", 0.0);
    // -------------------------------------------------
    // Solver
    // -------------------------------------------------
    cfg.solver.method   = j.at("solver").at("type").get<solver::Method>();
    cfg.solver.tol      = j.at("solver").at("tol").get<double>();
    cfg.solver.max_iter = j.at("solver").at("max_iter").get<int>();
    cfg.solver.omega    = j.at("solver").at("omega").get<double>();

    // -------------------------------------------------
    // Output paths (normalized)
    // -------------------------------------------------
    if (j.contains("paths"))
    {
        std::string raw = j.at("paths").at("output_root").get<std::string>();

        std::filesystem::path p(raw);

        if (!p.is_absolute()) {
            p = std::filesystem::current_path().parent_path() / p;
        }

        cfg.io.output_root = p.string();
    }

    // -------------------------------------------------
    // Boundary conditions
    // -------------------------------------------------
    cfg.boundary.clear();

    if (!j.contains("boundary_conditions")) {
        throw std::runtime_error("Config missing 'boundary_conditions'");
    }

    for (const auto& bcJson : j.at("boundary_conditions"))
    {        
        BoundaryConfig bc{};

        bc.group = bcJson.at("group").get<std::size_t>();

        bc.condition.type = bc::from_string(bcJson.at("type").get<std::string>());

        bc.condition.value = 0.0;
        bc.condition.flux  = 0.0;
        bc.condition.h     = 0.0;
        bc.condition.Tinf  = 0.0;

        switch (bc.condition.type)
        {
            case bc::Type::Dirichlet:
                bc.condition.value = bcJson.at("value").get<double>();
                break;

            case bc::Type::Neumann:
                bc.condition.flux = bcJson.at("flux").get<double>();
                break;

            case bc::Type::Convective:
                bc.condition.h = bcJson.at("h").get<double>();
                bc.condition.Tinf = bcJson.at("Tinf").get<double>();
                break;

            default:
                break;
        }

        cfg.boundary.push_back(bc);
    }

    // -------------------------------------------------
    // Verification (safe parsing)
    // -------------------------------------------------
    if (j.contains("verificationCase"))
    {
        const auto& v = j.at("verificationCase");

        cfg.verificationSuite.enabled = v.value("enabled", false);

        cfg.verificationSuite.plot_enabled = v.value("plot_enabled", false);

        cfg.verificationSuite.cases.clear();

        if (v.contains("cases"))
        {
            for (const auto& c : v.at("cases"))
            {
                VerificationCaseEntry entry;

                entry.name = c.at("case_name").get<std::string>();
                entry.params = c.value("params", nlohmann::json::object());

                cfg.verificationSuite.cases.push_back(std::move(entry));
            }
        }

        if (v.contains("output"))
        {
            cfg.verificationSuite.output.directory =
                v.at("output").value("directory",
                                    "output/verification");
        }
    }

    return cfg;
}
