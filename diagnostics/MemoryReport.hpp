#pragma once

#include <cstddef>

struct MemoryReport
{
    std::size_t processCurrent = 0;
    std::size_t processPeak = 0;

    std::size_t mesh = 0;
    std::size_t fields = 0;
    std::size_t fieldRegistry = 0;
    std::size_t discretization = 0;
    std::size_t linearSystem = 0;

    std::size_t estimatedTotal = 0;
};