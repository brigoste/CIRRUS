#pragma once

#include <memory>
#include "physics/ScalarTransport/ScalarTransportModel.hpp"
#include "config/SimulationConfig.hpp"

class PhysicsFactory
{
public:
    static std::unique_ptr<ScalarTransportModel> create(const PhysicsConfig& cfg);
};
