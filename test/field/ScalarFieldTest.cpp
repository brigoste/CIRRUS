#include "fields/ScalarField.hpp"
#include "mesh/QuadMesh2D.hpp"

#include <cassert>
#include <iostream>
#include <cmath>

void runScalarFieldTest()
{
    QuadMesh2D mesh(
        10,
        10,
        1.0,
        1.0
    );

    ScalarField temperature(
        "Temperature",
        mesh,
        FieldLocation::Cell,
        300.0
    );

    assert(temperature.size() == mesh.ncells());
    assert(std::abs(temperature[0] - 300.0) < 1e-12);

    temperature[0] = 350.0;
    assert(std::abs(temperature[0] - 350.0) < 1e-12);

    temperature.fill(400.0);

    for(std::size_t i = 0; i < temperature.size(); ++i)
    {
        assert(std::abs(temperature[i] - 400.0) < 1e-12);
    }

    std::cout << "ScalarField test passed.\n";
}