#include "fields/FieldRegistry.hpp"
#include "mesh/QuadMesh2D.hpp"

#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

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

    assert(registry.size() == 1);
    assert(registry.contains("Temperature"));

    assert(temperature.size() == mesh.ncells());
    assert(std::abs(temperature[0] - 300.0) < 1e-12);

    // --------------------------------------------------
    // Create vector field
    // --------------------------------------------------

    VectorField& velocity = registry.createVector(
        "Velocity",
        mesh,
        FieldLocation::Cell,
        Vector{1.0, 2.0, 3.0}
    );

    assert(registry.size() == 2);
    assert(registry.contains("Velocity"));

    assert(velocity.size() == mesh.ncells());

    assert(std::abs(velocity[0].x - 1.0) < 1e-12);
    assert(std::abs(velocity[0].y - 2.0) < 1e-12);
    assert(std::abs(velocity[0].z - 3.0) < 1e-12);

    // --------------------------------------------------
    // Access through typed interface
    // --------------------------------------------------

    ScalarField& temperature2 = registry.scalar("Temperature");
    VectorField& velocity2 = registry.vector("Velocity");

    assert(&temperature == &temperature2);
    assert(&velocity == &velocity2);

    // --------------------------------------------------
    // Verify modifications affect stored fields
    // --------------------------------------------------

    temperature2[0] = 350.0;

    assert(std::abs(temperature[0] - 350.0) < 1e-12);

    velocity2[0].x = 10.0;

    assert(std::abs(velocity[0].x - 10.0) < 1e-12);

    // --------------------------------------------------
    // Access through base interface
    // --------------------------------------------------
    FieldBase& base = registry.get("Temperature");

    assert(base.name() == "Temperature");
    assert(base.location() == FieldLocation::Cell);
    assert(base.size() == mesh.ncells());

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

    assert(duplicateCaught);

    // --------------------------------------------------
    // Missing field should fail
    // --------------------------------------------------

    bool missingCaught = false;

    try { registry.get("NotAField"); }
    catch(const std::runtime_error&) { missingCaught = true; }

    assert(missingCaught);

    // --------------------------------------------------
    // Clear registry
    // --------------------------------------------------

    registry.clear();

    assert(registry.size() == 0);

    assert(!registry.contains("Temperature"));
    assert(!registry.contains("Velocity"));

    std::cout << "FieldRegistry test passed.\n";
}
