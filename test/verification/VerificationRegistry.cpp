#include "VerificationRegistry.hpp"
#include <stdexcept>

VerificationRegistry& VerificationRegistry::instance()
{
    static VerificationRegistry instance;
    return instance;
}

void VerificationRegistry::registerCase(
    const std::string& name,
    Creator creator)
{
    registry_[name] = std::move(creator);
}

std::unique_ptr<VerificationCase>
VerificationRegistry::create(const VerificationConfig& cfg) const
{
    auto it = registry_.find(cfg.case_name);

    if (it == registry_.end())
    {
        throw std::runtime_error(
            "Unknown verification case: " + cfg.case_name);
    }

    return it->second(cfg);
}
