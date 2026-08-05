#pragma once

#include "Preconditioner.hpp"

#include <memory>
#include <string>

std::unique_ptr<Preconditioner> createPreconditioner(PreconditionerType type);
