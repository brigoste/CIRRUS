#pragma once

#include <memory>
#include "physics/PhysicsModel.hpp"
#include "config/SimulationConfig.hpp"

class PhysicsFactory
{
public:
    static std::unique_ptr<PhysicsModel> create(const PhysicsConfig& cfg);
};
