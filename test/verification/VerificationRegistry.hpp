#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <functional>

#include "tests/verification/VerificationCase.hpp"
#include "config/SimulationConfig.hpp"

// Plug-in style case registration
// string -> constructor mapping

class VerificationRegistry
{
public:
    using Creator = std::function<
        std::unique_ptr<VerificationCase>(const VerificationConfig&)
    >;

    static VerificationRegistry& instance();

    void registerCase(const std::string& name, Creator creator);

    std::unique_ptr<VerificationCase>
    create(const VerificationConfig& cfg) const;

private:
    std::unordered_map<std::string, Creator> registry_;
};
