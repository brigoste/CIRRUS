#include "fields/VectorField.hpp"
#include "mesh/QuadMesh2D.hpp"

#include <cassert>
#include <iostream>
#include <cmath>

void runVectorFieldTest()
{
    QuadMesh2D mesh(
        10,
        10,
        1.0,
        1.0
    );

    VectorField velocity(
        "Velocity",
        mesh,
        FieldLocation::Cell
    );

    // Size should match number of cells
    assert(velocity.size() == mesh.ncells());

    // Default constructor should initialize to zero vector
    assert(std::abs(velocity[0].x - 0.0) < 1e-12);
    assert(std::abs(velocity[0].y - 0.0) < 1e-12);
    assert(std::abs(velocity[0].z - 0.0) < 1e-12);

    assert(std::abs(velocity[0].magnitude() - 0.0) < 1e-12);

    // Modify a single vector
    velocity[0].x = 3.0;
    velocity[0].y = 4.0;
    velocity[0].z = 0.0;

    assert(std::abs(velocity[0].x - 3.0) < 1e-12);
    assert(std::abs(velocity[0].y - 4.0) < 1e-12);
    assert(std::abs(velocity[0].z - 0.0) < 1e-12);

    // 3-4-5 triangle
    assert(std::abs(velocity[0].magnitude() - 5.0) < 1e-12);

    // Fill the entire field
    Vector uniformVelocity;
    uniformVelocity.x = 1.0;
    uniformVelocity.y = 2.0;
    uniformVelocity.z = 3.0;

    velocity.fill(uniformVelocity);

    for (std::size_t i = 0; i < velocity.size(); ++i)
    {
        assert(std::abs(velocity[i].x - 1.0) < 1e-12);
        assert(std::abs(velocity[i].y - 2.0) < 1e-12);
        assert(std::abs(velocity[i].z - 3.0) < 1e-12);

        assert(std::abs(velocity[i].magnitudeSquared() - 14.0) < 1e-12);
    }

    std::cout << "VectorField test passed.\n";
}
