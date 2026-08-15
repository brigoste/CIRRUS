#pragma once

#include "test/verification/VerificationSummary.hpp"
#include <filesystem>
#include <string>

class Simulation;
class ScalarField;

class VerificationIO
{
public:

    static void writeCSV(
        const Simulation& sim,
        const ScalarField& phi,
        const std::filesystem::path& path);

    static void writeVTK(
        const Simulation& sim,
        const ScalarField& phi,
        const std::filesystem::path& path);

    static void writeReport(
        const VerificationSummary& result,
        const std::filesystem::path& file);
};