#include "fields/FieldRegistry.hpp"
#include "mesh/QuadMesh2D.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

namespace
{
    void check(bool condition, const char* message)
    {
        if (!condition) { throw std::runtime_error(message); }
    }
}

void runFieldRegistryTest()
{
    QuadMesh2D mesh(
        10,
        10,
        1.0,
        1.0
    );

    FieldRegistry registry;

    // --------------------------------------------------
    // Create scalar field
    // --------------------------------------------------

    ScalarField& temperature = registry.createScalar(
        "Temperature",
        mesh,
        FieldLocation::Cell,
        300.0
    );

    check(registry.size() == 1, "Registry size should be 1 (no test added or already had input)");
    check(registry.contains("Temperature"), "Registry should be of type Temperature");

    check(temperature.size() == mesh.ncells(), "Temperature field size incorrect");
    check(std::abs(temperature[0] - 300.0) < 1e-12,"Temperature initialization incorrect");

    // --------------------------------------------------
    // Create vector field
    // --------------------------------------------------

    VectorField& velocity = registry.createVector(
        "Velocity",
        mesh,
        FieldLocation::Cell,
        Vector{1.0, 2.0, 3.0}
    );

    check(registry.size() == 2, "Registry size should be 2");
    check(registry.contains("Velocity"), "Registry should be of type Velocity");

    check(velocity.size() == mesh.ncells(), "Velocity field size incorrect");

    check(std::abs(velocity[0].x - 1.0) < 1e-12, "Veliocity outside tolerance");
    check(std::abs(velocity[0].y - 2.0) < 1e-12, "Veliocity outside tolerance");
    check(std::abs(velocity[0].z - 3.0) < 1e-12, "Veliocity outside tolerance");

    // --------------------------------------------------
    // Access through typed interface
    // --------------------------------------------------

    ScalarField& temperature2 = registry.scalar("Temperature");
    VectorField& velocity2 = registry.vector("Velocity");

    check(&temperature == &temperature2, "Temperature references do not match");
    check(&velocity == &velocity2, "Velocity references do not match");

    // --------------------------------------------------
    // Verify modifications affect stored fields
    // --------------------------------------------------

    temperature2[0] = 350.0;

    check(std::abs(temperature[0] - 350.0) < 1e-12, "Temperature outside tolerance");

    velocity2[0].x = 10.0;

    check(std::abs(velocity[0].x - 10.0) < 1e-12, "Veliocity outside tolerance");

    // --------------------------------------------------
    // Access through base interface
    // --------------------------------------------------
    FieldBase& base = registry.get("Temperature");

    check(base.name() == "Temperature", "Incorrect base");
    check(base.location() == FieldLocation::Cell, "Location doesn't match");
    check(base.size() == mesh.ncells(), "Size not copied correctly");

    // --------------------------------------------------
    // Duplicate name should fail
    // --------------------------------------------------

    bool duplicateCaught = false;

    try
    {
        registry.createScalar(
            "Temperature",
            mesh,
            FieldLocation::Cell,
            500.0
        );
    }
    catch(const std::runtime_error&) { duplicateCaught = true; }

    check(duplicateCaught, "");

    // --------------------------------------------------
    // Missing field should fail
    // --------------------------------------------------

    bool missingCaught = false;

    try { registry.get("NotAField"); }
    catch(const std::runtime_error&) { missingCaught = true; }

    check(missingCaught, "Field is missing");

    // --------------------------------------------------
    // Clear registry
    // --------------------------------------------------

    registry.clear();

    check(registry.size() == 0, "Registry not cleared");

    check(!registry.contains("Temperature"), "Temperature Field still exists");
    check(!registry.contains("Velocity"), "Velocity Field still exists");

    std::cout << "FieldRegistry test passed.\n";
}
