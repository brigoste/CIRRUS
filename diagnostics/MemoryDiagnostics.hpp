#pragma once

#include <cstddef>

class MemoryDiagnostics
{
public:
    static std::size_t currentRSS();
    static std::size_t peakRSS();
};