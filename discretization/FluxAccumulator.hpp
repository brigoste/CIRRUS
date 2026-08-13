#pragma once

#include <cstddef>
#include <vector>

#include "discretization/CellResidual.hpp"
#include "discretization/FaceDiffusion.hpp"
#include "discretization/FaceConvection.hpp"
#include "discretization/BoundaryDiffusion.hpp"
#include "discretization/MatrixContribution.hpp"
#include "discretization/SourceContribution.hpp"

class FluxAccumulator
{
public:

    explicit FluxAccumulator(std::size_t nCells)
        : cells_(nCells)
    {}

    FluxAccumulator(const FluxAccumulator&) = delete;
    FluxAccumulator& operator=(const FluxAccumulator&) = delete;

    FluxAccumulator(FluxAccumulator&&) = default;
    FluxAccumulator& operator=(FluxAccumulator&&) = default;

    void reset()
    {
        diffusion_.clear();
        convection_.clear();
        matrixContributions_.clear();
        sourceContributions_.clear();
    }

    void addDiffusion(const FaceDiffusion& contribution) { diffusion_.push_back(contribution); }
    void addBoundaryDiffusion(const BoundaryDiffusion& contribution)
    {
        const auto P = contribution.P;
        const auto D = contribution.D;
        const auto value = contribution.value;

        sourceContributions_.push_back({
            P,
            D*value,
            -D
        });
    }
    void addConvection(const FaceConvection& contribution) { convection_.push_back(contribution); }
    void addSource( const SourceContribution& contribution ) { sourceContributions_.push_back(contribution); }
    void addMatrixContribution( const MatrixContribution& contribution ) { matrixContributions_.push_back(contribution); }

    const CellResidual& operator[](std::size_t i) const { return cells_[i]; }
    const auto& diffusion() const { return diffusion_; }
    const auto& convection() const { return convection_; }
    const auto& matrixContributions() const { return matrixContributions_; }
    const std::vector<SourceContribution>& sourceContributions() const { return sourceContributions_; }
    std::size_t size() const { return cells_.size(); }

private:

    std::vector<CellResidual> cells_;
    std::vector<FaceDiffusion> diffusion_;
    std::vector<FaceConvection> convection_;
    std::vector<MatrixContribution> matrixContributions_;
    std::vector<SourceContribution> sourceContributions_;
};
