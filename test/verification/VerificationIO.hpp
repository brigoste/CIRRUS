#pragma once

#include <filesystem>
#include <string>
#include <vector>

class Simulation;
class ScalarField;
class SimulationConfig;

class VerificationIO
{
public:

    // Write CSV of reconstructed field + residuals
    static void writeCSV( const Simulation& sim, const ScalarField& field, const std::filesystem::path& path );
    // Write L2 / Linf summary JSON
    static void writeSummary( const std::string& caseName, double l2, double linf, const std::filesystem::path& file);
};
