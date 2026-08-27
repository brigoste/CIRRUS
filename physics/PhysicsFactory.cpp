#include "physics/ScalarTransport/ScalarTransportModel.hpp"
#include "physics/ScalarTransport/HeatPhysicsModel.hpp"
#include "physics/ScalarTransport/AdvectionDiffusionPhysicsModel.hpp"
#include "physics/PhysicsFactory.hpp"

#include <stdexcept>
#include <nlohmann/json.hpp>

std::unique_ptr<ScalarTransportModel>
PhysicsFactory::create(const PhysicsConfig& cfg)
{
    switch (cfg.type)
    {
        case physics::PhysicsType::Heat:
            return std::make_unique<HeatPhysicsModel>(cfg.transferCoefficient);

        case physics::PhysicsType::AdvectionDiffusion:
            return std::make_unique<AdvectionDiffusionPhysicsModel>(cfg.gamma, cfg.rho, cfg.ux, cfg.uy, cfg.uz);
            
        default:
            throw std::runtime_error("Fluid physics model not implemented");
    }

    throw std::runtime_error("Unknown physics model");
}
