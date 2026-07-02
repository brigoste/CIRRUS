#pragma once

#include <vector>
#include <stdexcept>

#include "discretization/CellResidual.hpp"
#include "discretization/FaceDiffusion.hpp"
#include "discretization/FaceConvection.hpp"
#include "mesh/BoundaryPatchSystem.hpp"
#include "mesh/primitives/Face.hpp"

class FluxAccumulator
{
public:
    explicit FluxAccumulator(std::size_t nCells)
        : cells_(nCells) {}

    // =========================================================
    // LOCK DOWN COPY SEMANTICS
    // =========================================================
    FluxAccumulator(const FluxAccumulator&) = delete;
    FluxAccumulator& operator=(const FluxAccumulator&) = delete;

    FluxAccumulator(FluxAccumulator&&) = default;
    FluxAccumulator& operator=(FluxAccumulator&&) = default;

    // =========================================================
    // RESET (controlled reuse instead of reconstruction)
    // =========================================================
    void reset()
    {
        for (auto& c : cells_)
        {
            c.Su = 0.0;
            c.Sp = 0.0;
        }

        diffusion_.clear();
        convection_.clear();
    }

    // =========================================================
    // ADDERS
    // =========================================================
    void addDiffusion(std::size_t P, std::size_t N, double D)
    {
        if (N == Face::INVALID) { diffusion_.push_back({P, N, D, FaceType::Boundary}); }
        else { diffusion_.push_back({P, N, D, FaceType::Interior}); }
    }
    void addBoundaryDiffusion(std::size_t P, double D, double val)
    {
        cells_[P].Sp -= D;
        cells_[P].Su += D * val;   // Tw, or V, or P, whatever this is.
    }
    void addConvection(std::size_t P, std::size_t N, double F) { convection_.push_back({P, N, F}); }

    void addSource(std::size_t c, double Su, double Sp)
    {
        cells_[c].Su += Su;
        cells_[c].Sp += Sp;
    }

    // =========================================================
    // ACCESS
    // =========================================================
    const CellResidual& operator[](std::size_t i) const { return cells_[i]; }

    const auto& diffusion() const { return diffusion_; }
    const auto& convection() const { return convection_; }

    std::size_t size() const { return cells_.size(); }

private:
    std::vector<CellResidual> cells_;
    std::vector<FaceDiffusion> diffusion_ = {};
    std::vector<FaceConvection> convection_;
};
