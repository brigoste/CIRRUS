#pragma once

#include <memory>
#include <string>

#include "VerificationCase.hpp"

class VerificationCaseFactory
{
public:

    // static std::unique_ptr<VerificationCase> create(const std::string& name);

    static std::unique_ptr<VerificationCase> create(const std::string& name,
                                                    const nlohmann::json& params);
};
