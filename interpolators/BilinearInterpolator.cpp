#include "interpolators/BilinearInterpolator.hpp"

#include <stdexcept>
#include <cmath>
#include <vector>
#include <iostream>

double BilinearInterpolator::interpolate(
    const ScalarField& field,
    const Point& position
) const
{
    if (field.mesh().dim() != 2) { throw std::runtime_error( "BilinearInterpolator: only 2D meshes are supported." ); }

    return interpolate2D(field, position);
}

Vector BilinearInterpolator::interpolate(
    const VectorField& field,
    const Point& position
) const
{
    if (field.mesh().dim() != 2) { throw std::runtime_error( "BilinearInterpolator: only 2D meshes are supported." ); }

    return interpolate2D(field, position);
}

// ----- Bilinear Interpolation -----
double BilinearInterpolator::interpolate2D(
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

            return (w0 * field[nodes[0]]) + (w1 * field[nodes[1]]) + (w2 * field[nodes[2]]) + (w3 * field[nodes[3]]);
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

            // const std::size_t containingCell = mesh.findCell(position);

            // const Point& pc = mesh.cellCenter(containingCell);

            /*
             * For the current structured QuadMesh2D, determine
             * the surrounding cell-center rectangle by searching
             * the cell centers.
             */
            std::size_t c00 = MeshBase::INVALID;
            std::size_t c10 = MeshBase::INVALID;
            std::size_t c01 = MeshBase::INVALID;
            std::size_t c11 = MeshBase::INVALID;

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

            // std::cout << "Interpolation position: "
            //         << position[0] << ", "
            //         << position[1] << '\n';

            // std::cout << "Cell centers:\n";

            // for (std::size_t c = 0; c < mesh.ncells(); ++c)
            // {
            //     const Point& p = mesh.cellCenter(c);

            //     std::cout << c << ": "
            //             << p[0] << ", "
            //             << p[1] << '\n';
            // }

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

            if (c10 == MeshBase::INVALID || c01 == MeshBase::INVALID) { throw std::out_of_range("LinearInterpolator::interpolate2D: position lies outside cell-center interpolation range."); }

            for (std::size_t c = 0; c < mesh.ncells(); ++c)
            {
                const Point& p = mesh.cellCenter(c);

                if (p[0] == x1 && p[1] == y1)
                {
                    c11 = c;
                    break;
                }
            }

            if (c11 == MeshBase::INVALID) { throw std::runtime_error( "LinearInterpolator::interpolate2D: failed to construct cell-centered stencil."); }

            const double xi = (position[0] - x0) / (x1 - x0);

            const double eta = (position[1] - y0) / (y1 - y0);

            const double w00 = (1.0 - xi) * (1.0 - eta);
            const double w10 = xi * (1.0 - eta);
            const double w11 = xi * eta;
            const double w01 = (1.0 - xi) * eta;

            return (w00 * field[c00]) + (w10 * field[c10]) + (w11 * field[c11]) + (w01 * field[c01]);
        }

        case FieldLocation::Face:
            return interpolate2DFace(field, position);

        default:
            throw std::runtime_error("BilinearInterpolator::interpolate2D: unsupported field location.");
    }
}

Vector BilinearInterpolator::interpolate2D(
    const VectorField& field,
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

            return (w0 * field[nodes[0]]) + (w1 * field[nodes[1]]) + (w2 * field[nodes[2]]) + (w3 * field[nodes[3]]);
        }
        case FieldLocation::Face:
            return interpolate2DFace(field, position);

        case FieldLocation::Cell:
            throw std::runtime_error("BilinearInterpolator::interpolate2D: cell-centered fields not supported.");

        default:
             throw std::runtime_error("BilinearInterpolator::interpolate2D: unsupported field location.");
    

    }
}

double BilinearInterpolator::interpolate2DFace(
    const ScalarField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    struct FaceStencil
    {
        std::size_t f00 = MeshBase::INVALID;
        std::size_t f10 = MeshBase::INVALID;
        std::size_t f11 = MeshBase::INVALID;
        std::size_t f01 = MeshBase::INVALID;

        double x0 = 0.0;
        double x1 = 0.0;
        double y0 = 0.0;
        double y1 = 0.0;

        bool valid = false;
    };

    auto buildStencil = [&](bool xNormal) -> FaceStencil
    {
        FaceStencil stencil;

        bool foundX0 = false;
        bool foundX1 = false;
        bool foundY0 = false;
        bool foundY1 = false;

        /*
         * Find the four surrounding face centers belonging
         * to one face orientation.
         */
        for (std::size_t f = 0; f < mesh.nfaces(); ++f)
        {
            const Face& face = mesh.face(f);

            const bool isXNormal = std::abs(face.normal[0]) > 0.5;

            if (isXNormal != xNormal) { continue; }

            const double x = face.center[0];
            const double y = face.center[1];

            if (x <= position[0] && (!foundX0 || x > stencil.x0))
            {
                stencil.x0 = x;
                foundX0 = true;
            }

            if (x >= position[0] && (!foundX1 || x < stencil.x1))
            {
                stencil.x1 = x;
                foundX1 = true;
            }

            if (y <= position[1] && (!foundY0 || y > stencil.y0))
            {
                stencil.y0 = y;
                foundY0 = true;
            }

            if (y >= position[1] && (!foundY1 || y < stencil.y1))
            {
                stencil.y1 = y;
                foundY1 = true;
            }
        }

        if (!foundX0 || !foundX1 || !foundY0 || !foundY1) { return stencil; }

        /*
         * Locate the four faces at the corners.
         */
        constexpr double coordinateTolerance = 1.0e-12;

        for (std::size_t f = 0; f < mesh.nfaces(); ++f)
        {
            const Face& face = mesh.face(f);

            const bool isXNormal = std::abs(face.normal[0]) > 0.5;

            if (isXNormal != xNormal) { continue; }

            const double x = face.center[0];
            const double y = face.center[1];

            if (std::abs(x - stencil.x0) < coordinateTolerance && std::abs(y - stencil.y0) < coordinateTolerance)       { stencil.f00 = f; }
            else if ( std::abs(x - stencil.x1) < coordinateTolerance && std::abs(y - stencil.y0) < coordinateTolerance) { stencil.f10 = f; }
            else if ( std::abs(x - stencil.x1) < coordinateTolerance && std::abs(y - stencil.y1) < coordinateTolerance) { stencil.f11 = f; }
            else if ( std::abs(x - stencil.x0) < coordinateTolerance && std::abs(y - stencil.y1) < coordinateTolerance) { stencil.f01 = f; }
        }

        if (stencil.f00 == MeshBase::INVALID || stencil.f10 == MeshBase::INVALID || stencil.f11 == MeshBase::INVALID || stencil.f01 == MeshBase::INVALID) { return stencil; }

        stencil.valid = true;

        return stencil;
    };

    auto interpolateStencil = [&](const FaceStencil& stencil) -> double
    {
        const double xi = (position[0] - stencil.x0) / (stencil.x1 - stencil.x0);

        const double eta = (position[1] - stencil.y0) / (stencil.y1 - stencil.y0);

        const double w00 = (1.0 - xi) * (1.0 - eta);
        const double w10 = xi * (1.0 - eta);
        const double w11 = xi * eta;
        const double w01 = (1.0 - xi) * eta;

        return w00 * field[stencil.f00] 
             + w10 * field[stencil.f10] 
             + w11 * field[stencil.f11] 
             + w01 * field[stencil.f01];
    };

    const FaceStencil xStencil = buildStencil(true);
    const FaceStencil yStencil = buildStencil(false);

    const bool xValid = xStencil.valid;
    const bool yValid = yStencil.valid;

    if (!xValid && !yValid) { throw std::out_of_range( "BilinearInterpolator: position lies outside the face-centered interpolation range." ); }
    if (xValid && yValid) { return 0.5 * ( interpolateStencil(xStencil) + interpolateStencil(yStencil) ); }
    if (xValid) { return interpolateStencil(xStencil); }

    return interpolateStencil(yStencil);
}

Vector BilinearInterpolator::interpolate2DFace(
    const VectorField& field,
    const Point& position
) const
{
    const MeshBase& mesh = field.mesh();

    struct FaceStencil
    {
        std::size_t f00 = MeshBase::INVALID;
        std::size_t f10 = MeshBase::INVALID;
        std::size_t f11 = MeshBase::INVALID;
        std::size_t f01 = MeshBase::INVALID;

        double x0 = 0.0;
        double x1 = 0.0;
        double y0 = 0.0;
        double y1 = 0.0;

        bool valid = false;
    };

    auto buildStencil = [&](bool xNormal) -> FaceStencil
    {
        FaceStencil stencil;

        bool foundX0 = false;
        bool foundX1 = false;
        bool foundY0 = false;
        bool foundY1 = false;

        for (std::size_t f = 0; f < mesh.nfaces(); ++f)
        {
            const Face& face = mesh.face(f);

            const bool isXNormal = std::abs(face.normal[0]) > 0.5;

            if (isXNormal != xNormal) { continue; }

            const double x = face.center[0];
            const double y = face.center[1];

            if (x <= position[0] && (!foundX0 || x > stencil.x0))
            {
                stencil.x0 = x;
                foundX0 = true;
            }

            if (x >= position[0] && (!foundX1 || x < stencil.x1))
            {
                stencil.x1 = x;
                foundX1 = true;
            }

            if (y <= position[1] && (!foundY0 || y > stencil.y0))
            {
                stencil.y0 = y;
                foundY0 = true;
            }

            if (y >= position[1] && (!foundY1 || y < stencil.y1))
            {
                stencil.y1 = y;
                foundY1 = true;
            }
        }

        if (!foundX0 || !foundX1 || !foundY0 || !foundY1) { return stencil; }

        constexpr double coordinateTolerance = 1.0e-12;

        for (std::size_t f = 0; f < mesh.nfaces(); ++f)
        {
            const Face& face = mesh.face(f);

            const bool isXNormal =
                std::abs(face.normal[0]) > 0.5;

            if (isXNormal != xNormal) { continue; }

            const double x = face.center[0];
            const double y = face.center[1];

            if      (std::abs(x - stencil.x0) < coordinateTolerance && std::abs(y - stencil.y0) < coordinateTolerance)  { stencil.f00 = f; }
            else if ( std::abs(x - stencil.x1) < coordinateTolerance && std::abs(y - stencil.y0) < coordinateTolerance) { stencil.f10 = f; }
            else if ( std::abs(x - stencil.x1) < coordinateTolerance && std::abs(y - stencil.y1) < coordinateTolerance) { stencil.f11 = f; }
            else if ( std::abs(x - stencil.x0) < coordinateTolerance && std::abs(y - stencil.y1) < coordinateTolerance) { stencil.f01 = f; }
        }

        if (stencil.f00 == MeshBase::INVALID || stencil.f10 == MeshBase::INVALID || stencil.f11 == MeshBase::INVALID || stencil.f01 == MeshBase::INVALID) { return stencil; }

        stencil.valid = true;

        return stencil;
    };

    auto interpolateStencil =
        [&](const FaceStencil& stencil) -> Vector
    {
        const double xi = (position[0] - stencil.x0) / (stencil.x1 - stencil.x0);

        const double eta = (position[1] - stencil.y0) / (stencil.y1 - stencil.y0);

        const double w00 = (1.0 - xi) * (1.0 - eta);
        const double w10 = xi * (1.0 - eta);
        const double w11 = xi * eta;
        const double w01 = (1.0 - xi) * eta;

        return w00 * field[stencil.f00] 
             + w10 * field[stencil.f10] 
             + w11 * field[stencil.f11] 
             + w01 * field[stencil.f01];
    };

    const FaceStencil xStencil = buildStencil(true);
    const FaceStencil yStencil = buildStencil(false);

    if (!xStencil.valid && !yStencil.valid) { throw std::out_of_range( "BilinearInterpolator: position lies outside the face-centered interpolation range." ); }

    if (xStencil.valid && yStencil.valid) { return 0.5 * ( interpolateStencil(xStencil) + interpolateStencil(yStencil) ); }

    if (xStencil.valid) { return interpolateStencil(xStencil); }

    return interpolateStencil(yStencil);
}
