#pragma once

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

    static void writeSummary(
        const std::string& caseName,
        double l2,
        double linf,
        const std::filesystem::path& file);
};