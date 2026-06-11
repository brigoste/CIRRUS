#include "physics/PhysicsFactory.hpp"
#include "HeatPhysicsModel.hpp"

std::unique_ptr<PhysicsModel> PhysicsFactory::create(const PhysicsConfig& cfg)
{
    if (cfg.type == "heat")
    {
        return std::make_unique<HeatPhysicsModel>(cfg.k, cfg.Sp, cfg.Su);
    }
    if (cfg.type == "fluid")
    {
        throw std::runtime_error("Fluid physics model not yet implmented.");
    }

    throw std::runtime_error("Unknown physics model type: " + cfg.type);
}
