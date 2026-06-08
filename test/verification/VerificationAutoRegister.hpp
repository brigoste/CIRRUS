#pragma once

#include "VerificationRegistry.hpp"

struct VerificationAutoRegister
{
    VerificationAutoRegister(
        const std::string& name,
        VerificationRegistry::Creator creator)
    {
        VerificationRegistry::instance().registerCase(name, std::move(creator));
    }
};
