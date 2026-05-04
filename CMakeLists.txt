cmake_minimum_required(VERSION 3.16)

project(CIRRUS LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PYTHON_EXECUTABLE "C:/Users/E40112856/.julia/conda/3/x86_64/python.exe")

# --------------------------------------------------
# Include directories
# --------------------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# --------------------------------------------------
# Source files
# --------------------------------------------------
add_executable(CIRRUS
    src/main.cpp

    system/HeatSystem1D.cpp
    mesh/Mesh1D.cpp
    coeffs/Coefficients1D.cpp
    discretization/Discretization1D.cpp

    bc/DirichletBC.cpp
    bc/NeumannBC.cpp
    bc/ConvectiveBC.cpp
    bc/BoundaryFace.hpp

    solver/TDMA.cpp
)
target_include_directories(CIRRUS PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/system
    ${CMAKE_CURRENT_SOURCE_DIR}/mesh
    ${CMAKE_CURRENT_SOURCE_DIR}/coeffs
    ${CMAKE_CURRENT_SOURCE_DIR}/discretization
    ${CMAKE_CURRENT_SOURCE_DIR}/bc
    ${CMAKE_CURRENT_SOURCE_DIR}/solver
)

target_compile_definitions(CIRRUS PRIVATE
    PYTHON_EXECUTABLE="${PYTHON_EXECUTABLE}"
)
target_compile_definitions(CIRRUS PRIVATE
    PYTHON_EXECUTABLE="${PYTHON_EXECUTABLE}"
)
# add_custom_command(TARGET CIRRUS POST_BUILD
#     COMMAND ${CMAKE_COMMAND} -E copy
#     ${CMAKE_SOURCE_DIR}/plot.py
#     ${CMAKE_BINARY_DIR}/plot.py
# )
