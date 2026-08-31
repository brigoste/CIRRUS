#include "fields/VectorField.hpp"

#include <stdexcept>
#include <algorithm>
#include <utility>

VectorField::VectorField(
    std::string name,
    const MeshBase& mesh,
    FieldLocation location,
    const Vector& initialValue
)
:
FieldBase(std::move(name), mesh, location)
{
    switch (location_)
    {
        case FieldLocation::Cell:
            values_.assign(mesh_.ncells(), initialValue);
            break;

        case FieldLocation::Face:
            values_.assign(mesh_.nfaces(), initialValue);
            break;

        case FieldLocation::Node:
            values_.assign(mesh_.nnodes(), initialValue);
            break;

        default:
            throw std::runtime_error("Unknown field location.");
    }
}

std::size_t VectorField::size() const { return values_.size(); }
Vector& VectorField::operator[](std::size_t i) { return values_[i]; }
const Vector& VectorField::operator[](std::size_t i) const { return values_[i]; }

void VectorField::fill(const Vector& value)
{
    std::fill( values_.begin(),
               values_.end(),
               value );
}
