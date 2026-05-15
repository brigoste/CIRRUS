#pragma once

#include "mesh/MeshBase.hpp"
#include "linear_system/LinearSystem.hpp"
#include "physics/HeatEquationModel.hpp"
#include <limits>
#include <vector>
#include <cmath>
#include <stdexcept>
#include "discretization/CellResidual.hpp"
#include "discretization/FaceConvection.hpp"
#include "discretization/FaceDiffusion.hpp"

class FluxAccumulator
{
public:
    explicit FluxAccumulator(std::size_t nCells)
        : cells_(nCells) {}

    // =========================================================
    // 1. DIFFUSION (symmetric coupling)
    // =========================================================
    void addDiffusion(std::size_t P, std::size_t N, double D)
    {
        diffusion_.push_back({P, N, D});
    }

    // =========================================================
    // 2. CONVECTION (upwind flux)
    // =========================================================
    void addConvection(std::size_t P, std::size_t N, double F)
    {
        convection_.push_back({P, N, F});
    }

    // =========================================================
    // 3. BOUNDARY CONDITIONS
    // =========================================================
    void addBoundaryCondition(std::size_t P,
                              const BoundaryConditionDescriptor& bc,
                              double D,
                              double area)
    {
        switch (bc.type)
        {
            case BCType::Dirichlet:
                cells_[P].aP += D;
                cells_[P].Su += 2.0 * D * bc.value;
                break;

            case BCType::Neumann:
                cells_[P].Su += bc.flux * area;
                break;

            case BCType::Convective:
            {
                const double hA = bc.h * area;
                cells_[P].aP += hA;
                cells_[P].Su += hA * bc.Tinf;
                break;
            }
        }
    }

    // =========================================================
    // 4. SOURCE TERMS (volumetric physics)
    // =========================================================
    void addSource(std::size_t c, double Su, double Sp)
    {
        cells_[c].Su += Su;
        cells_[c].Sp += Sp;
    }

    // =========================================================
    // ACCESS
    // =========================================================
    const CellResidual& operator[](std::size_t i) const
    {
        return cells_[i];
    }

    const auto& diffusion() const { return diffusion_; }
    const auto& convection() const { return convection_; }

    std::size_t size() const { return cells_.size(); }

private:
    std::vector<CellResidual> cells_;

    std::vector<FaceDiffusion> diffusion_;
    std::vector<FaceConvection> convection_;
};
