#include "physics/PhysicsFactory.hpp"
#include "physics/HeatPhysicsModel.hpp"
#include "physics/AdvectionDiffusionPhysicsModel.hpp"

#include <stdexcept>
#include <nlohmann/json.hpp>

std::unique_ptr<PhysicsModel>
PhysicsFactory::create(const PhysicsConfig& cfg)
{
    switch (cfg.type)
    {
        case physics::PhysicsType::Heat:
            return std::make_unique<HeatPhysicsModel>(cfg.k);

        case physics::PhysicsType::AdvectionDiffusion:
            return std::make_unique<AdvectionDiffusionPhysicsModel>(cfg.gamma, cfg.rho, cfg.ux, cfg.uy, cfg.uz);
            
        default:
            throw std::runtime_error("Fluid physics model not implemented");
    }

    throw std::runtime_error("Unknown physics model");
}
