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
            throw std::runtime_error("LinearInterpolator: unsupported mesh dimension.");
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
            throw std::runtime_error("LinearInterpolator: unsupported mesh dimension.");
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

// ----- Bilinear Interpolation -----
double LinearInterpolator::interpolate2D(
    const ScalarField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    switch (field.location())
    {
        case FieldLocation::Node:
        {
            const std::size_t cell = mesh.findCell(position);

            std::vector<std::size_t> nodes;
            mesh.cellNodes(cell, nodes);

            if (nodes.size() != 4) { throw std::runtime_error( "LinearInterpolator::interpolate2D: expected four nodes for quadrilateral cell."); }

            const Point& p0 = mesh.node(nodes[0]);
            const Point& p1 = mesh.node(nodes[1]);
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

        case FieldLocation::Cell:
        {
            /*
             * Cell-centered interpolation requires the four
             * surrounding cell centers.
             *
             * This is intentionally handled separately from
             * node-centered interpolation because the stencil
             * is different.
             */

            const std::size_t containingCell = mesh.findCell(position);

            const Point& pc = mesh.cellCenter(containingCell);

            /*
             * For the current structured QuadMesh2D, determine
             * the surrounding cell-center rectangle by searching
             * the cell centers.
             */
            std::size_t c00 = Face::INVALID;
            std::size_t c10 = Face::INVALID;
            std::size_t c01 = Face::INVALID;
            std::size_t c11 = Face::INVALID;

            double x0 = -1.0;
            double x1 = -1.0;
            double y0 = -1.0;
            double y1 = -1.0;

            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const Point& p = mesh.cellCenter(c);

                if (p[0] <= position[0] && p[1] <= position[1])
                {
                    if (x0 < 0.0 || p[0] > x0)
                    {
                        x0 = p[0];
                        y0 = p[1];
                        c00 = c;
                    }
                }
            }

            /*
             * We need the other three corners of the
             * cell-center rectangle.
             */
            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const Point& p = mesh.cellCenter(c);

                if (p[0] == x0 && p[1] > y0)
                {
                    if (y1 < 0.0 || p[1] < y1)
                    {
                        y1 = p[1];
                        c01 = c;
                    }
                }

                if (p[0] > x0 && p[1] == y0)
                {
                    if (x1 < 0.0 || p[0] < x1)
                    {
                        x1 = p[0];
                        c10 = c;
                    }
                }
            }

            if (c10 == Face::INVALID || c01 == Face::INVALID) { throw std::out_of_range("LinearInterpolator::interpolate2D: position lies outside cell-center interpolation range."); }

            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const Point& p = mesh.cellCenter(c);

                if (p[0] == x1 && p[1] == y1)
                {
                    c11 = c;
                    break;
                }
            }

            if (c11 == Face::INVALID) { throw std::runtime_error( "LinearInterpolator::interpolate2D: failed to construct cell-centered stencil."); }

            const double xi = (position[0] - x0) / (x1 - x0);

            const double eta = (position[1] - y0) / (y1 - y0);

            const double w00 = (1.0 - xi) * (1.0 - eta);
            const double w10 = xi * (1.0 - eta);
            const double w11 = xi * eta;
            const double w01 = (1.0 - xi) * eta;

            return
                w00 * field[c00] +
                w10 * field[c10] +
                w11 * field[c11] +
                w01 * field[c01];
        }

        case FieldLocation::Face:
            throw std::runtime_error("LinearInterpolator::interpolate2D: face-centered interpolation not yet implemented.");

        default:
            throw std::runtime_error("LinearInterpolator::interpolate2D: unsupported field location.");
    }
}

Vector LinearInterpolator::interpolate2D(
    const VectorField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    if (field.location() != FieldLocation::Node) { throw std::runtime_error("LinearInterpolator::interpolate2D: node-centered fields only."); }

    const std::size_t cell = mesh.findCell(position);

    std::vector<std::size_t> nodes;
    mesh.cellNodes(cell, nodes);

    if (nodes.size() != 4) { throw std::runtime_error("LinearInterpolator::interpolate2D: expected four nodes for quadrilateral cell."); }

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
