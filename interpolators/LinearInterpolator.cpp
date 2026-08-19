#include "interpolators/LinearInterpolator.hpp"

#include <stdexcept>
#include <cmath>
#include <vector>
#include <iostream>

double LinearInterpolator::interpolate(
    const ScalarField& field,
    const Point& position
) const
{
    if (field.mesh().dim() != 1)
    {
        throw std::runtime_error( "LinearInterpolator: only 1D meshes are supported." );
    }

    return interpolate1D(field, position);
}

Vector LinearInterpolator::interpolate(
    const VectorField& field,
    const Point& position
) const
{
    if (field.mesh().dim() != 1)
    {
        throw std::runtime_error( "LinearInterpolator: only 1D meshes are supported." );
    }

    return interpolate1D(field, position);
}

// ------------- INTERPOLATION IMPLENTATIONS ------------------------------

double LinearInterpolator::interpolate1D(
    const ScalarField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    const double x = position[0];

    // Determine number of interpolation points.
    std::size_t n = 0;

    switch (field.location())
    {
        case FieldLocation::Node:
            n = mesh.nnodes();
            break;

        case FieldLocation::Face:
            n = mesh.nfaces();
            break;

        case FieldLocation::Cell:
            n = mesh.ncells();
            break;

        default:
            throw std::runtime_error( "LinearInterpolator: unsupported field location." );
    }

    if (n < 2) { throw std::runtime_error( "LinearInterpolator: at least two field locations are required."  ); }

    // Get coordinate of interpolation point i.
    auto coordinate = [&](std::size_t i) -> double
    {
        switch (field.location())
        {
            case FieldLocation::Node:
                return mesh.node(i)[0];

            case FieldLocation::Face:
                return mesh.face(i).center[0];

            case FieldLocation::Cell:
                return mesh.cellCenter(i)[0];

            default:
                throw std::runtime_error( "LinearInterpolator: unsupported field location." );
        }
    };

    // Check domain of the field's interpolation locations.
    const double xMin = coordinate(0);
    const double xMax = coordinate(n - 1);

    if (x < xMin || x > xMax) { throw std::out_of_range( "LinearInterpolator: position lies outside interpolation range." ); }

    // Locate the interval containing x.
    for (std::size_t i = 0; i < n - 1; ++i)
    {
        const double x0 = coordinate(i);
        const double x1 = coordinate(i + 1);

        if (x >= x0 && x <= x1)
        {
            const double alpha = (x - x0) / (x1 - x0);

            return (1.0 - alpha) * field[i] + alpha * field[i + 1];
        }
    }

    throw std::runtime_error( "LinearInterpolator: failed to locate interpolation interval." );
}

Vector LinearInterpolator::interpolate1D(
    const VectorField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    const double x = position[0];

    std::size_t n = 0;

    switch (field.location())
    {
        case FieldLocation::Node:
            n = mesh.nnodes();
            break;

        case FieldLocation::Face:
            n = mesh.nfaces();
            break;

        case FieldLocation::Cell:
            n = mesh.ncells();
            break;

        default:
            throw std::runtime_error("LinearInterpolator: unsupported field location.");
    }

    if (n < 2) { throw std::runtime_error("LinearInterpolator: at least two field locations are required."); }

    auto coordinate = [&](std::size_t i) -> double
    {
        switch (field.location())
        {
            case FieldLocation::Node:
                return mesh.node(i)[0];

            case FieldLocation::Face:
                return mesh.face(i).center[0];

            case FieldLocation::Cell:
                return mesh.cellCenter(i)[0];

            default:
                throw std::runtime_error("LinearInterpolator: unsupported field location.");
        }
    };

    const double xMin = coordinate(0);
    const double xMax = coordinate(n - 1);

    if (x < xMin || x > xMax) { throw std::out_of_range( "LinearInterpolator: position lies outside interpolation range."); }

    for (std::size_t i = 0; i < n - 1; ++i)
    {
        const double x0 = coordinate(i);
        const double x1 = coordinate(i + 1);

        if (x >= x0 && x <= x1)
        {
            const double alpha = (x - x0) / (x1 - x0);

            return (1.0 - alpha) * field[i] + alpha * field[i + 1];
        }
    }

    throw std::runtime_error(
        "LinearInterpolator: failed to locate interpolation interval.");
}
