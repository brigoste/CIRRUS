#include "interpolators/LinearInterpolator.hpp"

#include <stdexcept>
#include <cmath>
#include <vector>
#include <iostream>


// ------------- DISPATCHERS BASED ON MESH DIMENSIONS ------------------------
double LinearInterpolator::interpolate(
    const ScalarField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    switch (mesh.dim())
    {
        case 1:
            return interpolate1D(field, position);

        case 2:
            return interpolate2D(field, position);

        default:
            throw std::runtime_error(
                "LinearInterpolator: unsupported mesh dimension.");
    }
}

Vector LinearInterpolator::interpolate(
    const VectorField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    switch (mesh.dim())
    {
        case 1:
            return interpolate1D(field, position);

        case 2:
            return interpolate2D(field, position);

        default:
            throw std::runtime_error(
                "LinearInterpolator: unsupported mesh dimension.");
    }
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
            throw std::runtime_error(
                "LinearInterpolator: unsupported field location.");
    }

    if (n < 2)
    {
        throw std::runtime_error(
            "LinearInterpolator: at least two field locations are required.");
    }

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
                throw std::runtime_error(
                    "LinearInterpolator: unsupported field location.");
        }
    };

    const double xMin = coordinate(0);
    const double xMax = coordinate(n - 1);

    if (x < xMin || x > xMax)
    {
        throw std::out_of_range(
            "LinearInterpolator: position lies outside interpolation range.");
    }

    for (std::size_t i = 0; i < n - 1; ++i)
    {
        const double x0 = coordinate(i);
        const double x1 = coordinate(i + 1);

        if (x >= x0 && x <= x1)
        {
            const double alpha = (x - x0) / (x1 - x0);

            return
                (1.0 - alpha) * field[i]
                + alpha * field[i + 1];
        }
    }

    throw std::runtime_error(
        "LinearInterpolator: failed to locate interpolation interval.");
}

// ----- Bilinear Interpolation -----
double LinearInterpolator::interpolate2D(
    const ScalarField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    if (field.location() != FieldLocation::Node)
    {
        throw std::runtime_error("LinearInterpolator::interpolate2D: node-centered fields only.");
    }

    const std::size_t cell = mesh.findCell(position);

    std::cout
        << "2D interpolation:\n"
        << "  position = (" << position[0] << ", " << position[1] << ")\n"
        << "  cell     = " << cell << "\n";

    std::vector<std::size_t> nodes;
    mesh.cellNodes(cell, nodes);

    if (nodes.size() != 4)
    {
        throw std::runtime_error("LinearInterpolator::interpolate2D: expected four nodes for quadrilateral cell.");
    }

    const Point& p0 = mesh.node(nodes[0]);
    const Point& p1 = mesh.node(nodes[1]);
    // const Point& p2 = mesh.node(nodes[2]);
    const Point& p3 = mesh.node(nodes[3]);

    // QuadMesh2D currently consists of axis-aligned rectangular cells.
    // Therefore physical coordinates map directly to normalized
    // reference-cell coordinates.
    const double xi = (position[0] - p0[0]) / (p1[0] - p0[0]);

    const double eta = (position[1] - p0[1]) / (p3[1] - p0[1]);

    const double w0 = (1.0 - xi) * (1.0 - eta);
    const double w1 = xi * (1.0 - eta);
    const double w2 = xi * eta;
    const double w3 = (1.0 - xi) * eta;

    return
        w0 * field[nodes[0]] +
        w1 * field[nodes[1]] +
        w2 * field[nodes[2]] +
        w3 * field[nodes[3]];
}

Vector LinearInterpolator::interpolate2D(
    const VectorField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    if (field.location() != FieldLocation::Node)
    {
        throw std::runtime_error("LinearInterpolator::interpolate2D: node-centered fields only.");
    }

    const std::size_t cell = mesh.findCell(position);

    std::vector<std::size_t> nodes;
    mesh.cellNodes(cell, nodes);

    if (nodes.size() != 4)
    {
        throw std::runtime_error("LinearInterpolator::interpolate2D: expected four nodes for quadrilateral cell.");
    }

    const Point& p0 = mesh.node(nodes[0]);
    const Point& p1 = mesh.node(nodes[1]);
    // const Point& p2 = mesh.node(nodes[2]);
    const Point& p3 = mesh.node(nodes[3]);

    const double xi = (position[0] - p0[0]) / (p1[0] - p0[0]);

    const double eta = (position[1] - p0[1]) / (p3[1] - p0[1]);

    const double w0 = (1.0 - xi) * (1.0 - eta);
    const double w1 = xi * (1.0 - eta);
    const double w2 = xi * eta;
    const double w3 = (1.0 - xi) * eta;

    return
        w0 * field[nodes[0]] +
        w1 * field[nodes[1]] +
        w2 * field[nodes[2]] +
        w3 * field[nodes[3]];
}
