cmake_minimum_required(VERSION 3.20)
project(CIRRUS LANGUAGES CXX)

# ----------------------------------------
# Global settings
# ----------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Warnings (practical defaults)
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# ----------------------------------------
# Generate config header from config.h.in
# ----------------------------------------
configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_BINARY_DIR}/config.h
)

# Make generated header visible to target
include_directories(${PROJECT_BINARY_DIR})

# ----------------------------------------
# Collect all source files
# ----------------------------------------
file(GLOB_RECURSE CIRRUS_SOURCES
    ${PROJECT_SOURCE_DIR}/main.cpp
    ${PROJECT_SOURCE_DIR}/bc/*.cpp
    ${PROJECT_SOURCE_DIR}/discretization/*.cpp
    ${PROJECT_SOURCE_DIR}/linear_system/*.cpp
    ${PROJECT_SOURCE_DIR}/mesh/*.cpp
    ${PROJECT_SOURCE_DIR}/physics/*.cpp
    ${PROJECT_SOURCE_DIR}/Solver/*.cpp
    ${PROJECT_SOURCE_DIR}/system/*.cpp
    ${PROJECT_SOURCE_DIR}/utils/*.cpp
)

# ----------------------------------------
# Executable
# ----------------------------------------
add_executable(CIRRUS ${CIRRUS_SOURCES})

# ----------------------------------------
# Include directories
# ----------------------------------------
target_include_directories(CIRRUS PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}   # for config.h
)

# ----------------------------------------
# IDE source grouping (optional but useful)
# ----------------------------------------
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${CIRRUS_SOURCES})

# cmake_minimum_required(VERSION 3.20)

# project(CIRRUS LANGUAGES CXX)

# # -----------------------------
# # C++ Standard
# # -----------------------------
# set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_EXTENSIONS OFF)

# # -----------------------------
# # Build type
# # -----------------------------
# if(NOT CMAKE_BUILD_TYPE)
#     set(CMAKE_BUILD_TYPE Release)
# endif()

# # -----------------------------
# # Python (optional)
# # -----------------------------
# set(PYTHON_EXECUTABLE "C:/Users/E40112856/.julia/conda/3/x86_64/python.exe")

# # -----------------------------
# # Core libraries
# # -----------------------------

# add_library(linear_system
#     linear_system/LinearSystem.cpp
# )

# target_include_directories(linear_system PUBLIC
#     ${CMAKE_SOURCE_DIR}
# )

# # -----------------------------
# add_library(mesh
#     mesh/Mesh1D.cpp
#     mesh/Mesh2D.cpp
# )

# target_include_directories(mesh PUBLIC
#     ${CMAKE_SOURCE_DIR}
# )

# # -----------------------------
# add_library(bc
#     bc/DirichletBC.cpp
#     bc/NeumannBC.cpp
#     bc/ConvectiveBC.cpp
# )

# target_include_directories(bc PUBLIC
#     ${CMAKE_SOURCE_DIR}
# )

# target_link_libraries(bc PUBLIC
#     mesh
# )

# # -----------------------------
# add_library(discretization
#     discretization/Discretization.cpp
#     discretization/FiniteVolumeOperator.cpp
# )

# target_include_directories(discretization PUBLIC
#     ${CMAKE_SOURCE_DIR}
# )

# # -----------------------------
# add_library(solver
#     Solver/CG.cpp
#     Solver/GaussSeidel.cpp
#     Solver/SOR.cpp
#     Solver/TDMA.cpp
# )

# target_include_directories(solver PUBLIC
#     ${CMAKE_SOURCE_DIR}
# )

# target_link_libraries(solver PUBLIC
#     linear_system
# )

# # -----------------------------
# add_library(system
#     system/HeatCase1D.cpp
#     system/HeatSystem.cpp
#     system/HeatSystem1D.cpp
#     system/SimulationCase.cpp
# )

# target_include_directories(system PUBLIC
#     ${CMAKE_SOURCE_DIR}
# )

# target_link_libraries(system PUBLIC
#     bc
#     mesh
#     solver
#     discretization
#     linear_system
#     physics
#     utils
# )

# # -----------------------------
# # Executable
# # -----------------------------
# add_executable(CIRRUS
#     src/main.cpp
# )

# target_include_directories(CIRRUS PRIVATE
#     ${CMAKE_SOURCE_DIR}
# )

# target_link_libraries(CIRRUS PRIVATE
#     system
#     solver
#     bc
#     mesh
#     discretization
#     linear_system
#     physics
#     utils
# )

# # -----------------------------
# # Compiler warnings
# # -----------------------------
# target_compile_options(CIRRUS PRIVATE
#     -Wall
#     -Wextra
#     -Wpedantic
# )

# # -----------------------------
# # Generated config header (optional)
# # -----------------------------
# configure_file(
#     ${CMAKE_SOURCE_DIR}/config.h.in
#     ${CMAKE_BINARY_DIR}/config.h
# )

# target_include_directories(CIRRUS PRIVATE
#     ${CMAKE_BINARY_DIR}
# )
