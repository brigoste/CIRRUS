#include "mesh/BoundaryPatchSystem.hpp"

void BoundaryPatchSystem::setGroup(std::size_t group, const Condition& bc)
{
    groupBC_[group] = bc;
}

const BoundaryPatchSystem::Condition*
BoundaryPatchSystem::getGroup(std::size_t group) const
{
    auto it = groupBC_.find(group);
    return (it == groupBC_.end()) ? nullptr : &it->second;
}
