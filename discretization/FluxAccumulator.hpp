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
        for (auto& c : cells_)
        {
            c.Su = 0.0;
            c.Sp = 0.0;
        }

        diffusion_.clear();
        convection_.clear();
        matrixContributions_.clear();
    }

    void addDiffusion(const FaceDiffusion& contribution) { diffusion_.push_back(contribution); }
    void addBoundaryDiffusion(const BoundaryDiffusion& contribution)
    {
        const auto P = contribution.P;
        const auto D = contribution.D;
        const auto value = contribution.value;

        cells_[P].Sp -= D;
        cells_[P].Su += D * value;
    }
    void addConvection(const FaceConvection& contribution) { convection_.push_back(contribution); }
    void addSource( const SourceContribution& contribution )
    {
        const auto c = contribution.cell;

        cells_[c].Su += contribution.Su;
        cells_[c].Sp += contribution.Sp;
    }
    void addMatrixContribution( const MatrixContribution& contribution ) { matrixContributions_.push_back(contribution); }

    const CellResidual& operator[](std::size_t i) const { return cells_[i]; }
    const auto& diffusion() const { return diffusion_; }
    const auto& convection() const { return convection_; }
    const auto& matrixContributions() const { return matrixContributions_; }
    std::size_t size() const { return cells_.size(); }

private:

    std::vector<CellResidual> cells_;
    std::vector<FaceDiffusion> diffusion_;
    std::vector<FaceConvection> convection_;
    std::vector<MatrixContribution> matrixContributions_;
};
