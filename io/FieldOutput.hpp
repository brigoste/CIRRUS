#pragma once

#include <string>

class ScalarField;
class PointField;

struct FieldOutput
{
    std::string name;
    const ScalarField* cellField = nullptr;
    const PointField* pointField = nullptr;
};