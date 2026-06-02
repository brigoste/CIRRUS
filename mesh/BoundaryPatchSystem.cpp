#include "mesh/BoundaryPatchSystem.hpp"

#include <stdexcept>

// =========================================================
// Registration
// =========================================================

void BoundaryPatchSystem::set(std::size_t faceIndex, const Condition& bc)
{
    bcMap_[faceIndex] = bc;
}

// =========================================================
// Query
// =========================================================

bool BoundaryPatchSystem::has(std::size_t faceIndex) const
{
    return bcMap_.find(faceIndex) != bcMap_.end();
}

const BoundaryPatchSystem::Condition* BoundaryPatchSystem::get(std::size_t faceIndex) const
{
    auto it = bcMap_.find(faceIndex);
    if (it == bcMap_.end())
        return nullptr;

    return &it->second;
}
