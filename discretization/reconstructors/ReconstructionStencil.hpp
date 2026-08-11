#pragma once

#include <cstddef>
#include <vector>
#include <utility>

struct ReconstructionStencil
{
    std::vector<std::pair<std::size_t, double>> weights;
};
