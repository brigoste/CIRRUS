#include "fields/FieldBase.hpp"

#include <utility>

FieldBase::FieldBase(
    std::string name,
    const MeshBase& mesh,
    FieldLocation location)
    :
    mesh_(mesh),
    name_(std::move(name)),
    location_(location)
{
}
