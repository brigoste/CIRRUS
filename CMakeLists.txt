cmake_minimum_required(VERSION 3.20)

project(CIRRUS LANGUAGES CXX)

# -----------------------------
# C++ standard
# -----------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# -----------------------------
# Build type
# -----------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# -----------------------------
# Include directories
# -----------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/system
    ${CMAKE_SOURCE_DIR}/bc
    ${CMAKE_SOURCE_DIR}/mesh
    ${CMAKE_SOURCE_DIR}/discretization
    ${CMAKE_SOURCE_DIR}/Solver
    ${CMAKE_SOURCE_DIR}/utils
)

# -----------------------------
# Source files
# -----------------------------
set(SOURCES
    src/main.cpp

    system/HeatSystemBase.cpp
    system/HeatSystem1D.cpp
    system/LinearSystem.cpp

    discretization/Discretization1D.cpp
    mesh/Mesh1D.cpp

    bc/DirichletBC.cpp
    bc/NeumannBC.cpp
    bc/ConvectiveBC.cpp

    Solver/TDMA.cpp
    Solver/GaussSeidel.cpp
    Solver/SOR.cpp
)

# -----------------------------
# Executable
# -----------------------------
add_executable(CIRRUS ${SOURCES})

# -----------------------------
# Compiler warnings
# -----------------------------
target_compile_options(CIRRUS PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)
