#include "physics/PhysicsFactory.hpp"
#include "HeatPhysicsModel.hpp"

std::unique_ptr<PhysicsModel> PhysicsFactory::create(const PhysicsConfig& cfg)
{
    if (cfg.type == "heat")
    {
        return std::make_unique<HeatPhysicsModel>(cfg.k);
    }

    throw std::runtime_error("Unknown physics model type: " + cfg.type);
}
