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
    ${PROJECT_SOURCE_DIR}/config/*.cpp
    ${PROJECT_SOURCE_DIR}/bc/*.cpp
    ${PROJECT_SOURCE_DIR}/discretization/*.cpp
    ${PROJECT_SOURCE_DIR}/fields/*.cpp
    ${PROJECT_SOURCE_DIR}/io/*.cpp
    ${PROJECT_SOURCE_DIR}/linear_system/*.cpp
    ${PROJECT_SOURCE_DIR}/postprocessing/*.cpp
    ${PROJECT_SOURCE_DIR}/physics/*.cpp
    ${PROJECT_SOURCE_DIR}/simulation/*.cpp
    ${PROJECT_SOURCE_DIR}/solver/*.cpp
    ${PROJECT_SOURCE_DIR}/solver/preconditioners/*.cpp
    ${PROJECT_SOURCE_DIR}/test/verification/*.cpp
    ${PROJECT_SOURCE_DIR}/test/verification/ManufacturedCases/*.cpp
    ${PROJECT_SOURCE_DIR}/utils/*.cpp

    # Update as we get 2D/3D meshes working
    # ${PROJECT_SOURCE_DIR}/mesh/*.cpp
    ${PROJECT_SOURCE_DIR}/mesh/Mesh1D.cpp
    ${PROJECT_SOURCE_DIR}/mesh/QuadMesh2D.cpp
    ${PROJECT_SOURCE_DIR}/mesh/BoundaryPatchSystem.cpp
    ${PROJECT_SOURCE_DIR}/mesh/primitives/Point.cpp
    ${PROJECT_SOURCE_DIR}/mesh/MeshGeometry.cpp

)

# ----------------------------------------
# JSON implmentation for problem configuration
# ----------------------------------------

# ----------------------------------------
# nlohmann_json (MSYS2 / system package)
# ----------------------------------------
find_path(NLOHMANN_JSON_INCLUDE_DIR
    nlohmann/json.hpp
    PATHS
        /usr/include
        /usr/local/include
        /mingw64/include
        /ucrt64/include
    REQUIRED
)

message(STATUS "nlohmann_json include dir: ${NLOHMANN_JSON_INCLUDE_DIR}")

include_directories(${NLOHMANN_JSON_INCLUDE_DIR})

# ----------------------------------------
# Executable - main
# ----------------------------------------
add_library(CIRRUS_core STATIC ${CIRRUS_SOURCES})

add_executable(CIRRUS
    src/main.cpp
)

target_link_libraries(CIRRUS
    PRIVATE CIRRUS_core
)

# ----------------------------------------
# Executable - testing
# ----------------------------------------

# Main test suite
file(GLOB_RECURSE TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/test/*.cpp
)

# Remove discretization tests from main suite
list(FILTER TEST_SOURCES EXCLUDE REGEX ".*/test/discretization/.*")

add_executable(CIRRUS_tests
    ${TEST_SOURCES}
)

target_link_libraries(CIRRUS_tests
    PRIVATE CIRRUS_core
)


# ----------------------------------------
# Executable - discretization tests
# ----------------------------------------

file(GLOB_RECURSE DISCRETIZATION_TEST_SOURCES
    ${PROJECT_SOURCE_DIR}/test/discretization/*.cpp
)

add_executable(CIRRUS_discretization_tests
    ${DISCRETIZATION_TEST_SOURCES}
)

target_link_libraries(CIRRUS_discretization_tests
    PRIVATE CIRRUS_core
)

# ----------------------------------------
# Include directories
# ----------------------------------------
target_include_directories(CIRRUS_core PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

# ----------------------------------------
# IDE source grouping (optional but useful)
# ----------------------------------------
source_group(TREE ${PROJECT_SOURCE_DIR} FILES ${CIRRUS_SOURCES})
