#include "diagnostics/MemoryDiagnostics.hpp"

#include <stdexcept>

#ifdef _WIN32

#include <windows.h>
#include <psapi.h>

#else

#include <fstream>
#include <sstream>
#include <string>

#endif


std::size_t MemoryDiagnostics::currentRSS()
{
#ifdef _WIN32

    PROCESS_MEMORY_COUNTERS counters{};

    if (!GetProcessMemoryInfo(
            GetCurrentProcess(),
            &counters,
            sizeof(counters)))
    {
        throw std::runtime_error(
            "MemoryDiagnostics: failed to query current RSS."
        );
    }

    return static_cast<std::size_t>(counters.WorkingSetSize);

#else

    std::ifstream status("/proc/self/status");

    if (!status)
    {
        throw std::runtime_error(
            "MemoryDiagnostics: failed to open /proc/self/status."
        );
    }

    std::string line;

    while (std::getline(status, line))
    {
        if (line.rfind("VmRSS:", 0) == 0)
        {
            std::size_t value = 0;
            std::string unit;

            std::istringstream stream(line.substr(6));

            stream >> value >> unit;

            if (!stream)
            {
                throw std::runtime_error(
                    "MemoryDiagnostics: failed to parse VmRSS."
                );
            }

            return value * 1024;
        }
    }

    throw std::runtime_error(
        "MemoryDiagnostics: VmRSS not found."
    );

#endif
}


std::size_t MemoryDiagnostics::peakRSS()
{
#ifdef _WIN32

    PROCESS_MEMORY_COUNTERS counters{};

    if (!GetProcessMemoryInfo(
            GetCurrentProcess(),
            &counters,
            sizeof(counters)))
    {
        throw std::runtime_error(
            "MemoryDiagnostics: failed to query peak RSS."
        );
    }

    return static_cast<std::size_t>(
        counters.PeakWorkingSetSize
    );

#else

    std::ifstream status("/proc/self/status");

    if (!status)
    {
        throw std::runtime_error(
            "MemoryDiagnostics: failed to open /proc/self/status."
        );
    }

    std::string line;

    while (std::getline(status, line))
    {
        if (line.rfind("VmHWM:", 0) == 0)
        {
            std::size_t value = 0;
            std::string unit;

            std::istringstream stream(line.substr(6));

            stream >> value >> unit;

            if (!stream)
            {
                throw std::runtime_error(
                    "MemoryDiagnostics: failed to parse VmHWM."
                );
            }

            return value * 1024;
        }
    }

    throw std::runtime_error(
        "MemoryDiagnostics: VmHWM not found."
    );

#endif
}