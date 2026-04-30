cmake_minimum_required(VERSION 3.16)

project(HT_CFD_Package LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(PYTHON_EXECUTABLE "C:/Users/E40112856/.julia/conda/3/x86_64/python.exe")

# --------------------------------------------------
# Include directories (IMPORTANT FIX)
# --------------------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}
)

# --------------------------------------------------
# Source files
# --------------------------------------------------
add_executable(HT_CFD_Package
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

    src/Plot.py
)

target_compile_definitions(HT_CFD_Package PRIVATE
    PYTHON_EXECUTABLE="${PYTHON_EXECUTABLE}"
)
add_custom_command(TARGET HT_CFD_Package POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/plot.py
    ${CMAKE_BINARY_DIR}/plot.py
)
