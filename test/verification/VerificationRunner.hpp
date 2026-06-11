#pragma once

#include <vector>
#include "config/SimulationConfig.hpp"

// Computes:
//  exact field
//  error field
//  norms
//  outputs

class MeshBase;
class LinearSystem;
class VerificationCase;


struct VerificationConfig;
struct ErrorNormResults;

class VerificationRunner
{
public:

    static void run(
        const MeshBase& mesh,
        const std::vector<double>& solution,
        const LinearSystem& sys,
        const VerificationCase& verificationCase,
        const VerificationConfig& config);

private:

    static std::vector<double> buildExactField(
        const MeshBase& mesh,
        const VerificationCase& verificationCase);

    static std::vector<double> buildErrorField(
        const std::vector<double>& numerical,
        const std::vector<double>& exact);

    static void printSummary(
        const ErrorNormResults& norms);
};
