#pragma once

#include <filesystem>

struct PathContext
{
    std::filesystem::path projectRoot;
    
    std::filesystem::path outputRoot;
    std::filesystem::path verificationRoot;
    std::filesystem::path scriptRoot;

    std::filesystem::path pythonExecutable;
};
