#include "fields/ScalarField.hpp"

#include "interpolators/LinearInterpolator.hpp"
#include "interpolators/BilinearInterpolator.hpp"

#include <stdexcept>
#include <algorithm>
#include <utility>

ScalarField::ScalarField(
    std::string name,
    const MeshBase& mesh,
    FieldLocation location,
    double initialValue
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

std::size_t ScalarField::size() const { return values_.size(); }
double& ScalarField::operator[](std::size_t i) { return values_[i]; }
const double& ScalarField::operator[](std::size_t i) const { return values_[i]; }

void ScalarField::fill(double value) 
{ 
    std::fill( values_.begin(), 
               values_.end(), 
               value ); 
}

// Determining intra-mesh point values via interpolation from nearby points.
double ScalarField::operator()(const Point& position) const
{
    // This will only work for strucutred meshes. For unstructed, we'll want to use properties of the mesh.
    switch (mesh_.dim())
    {
        case 1:
        {
            LinearInterpolator interpolator;
            return interpolator.interpolate(*this, position);
        }
        case 2:
        {
            BilinearInterpolator interpolator;
            return interpolator.interpolate(*this, position);
        }

        default:
            throw std::runtime_error("ScalarField interpolation is not supported for mesh type: " + mesh_.dim());
    }
}
