#!/usr/bin/env bash

set -e

############################################################
# Project configuration
############################################################

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

cd "$PROJECT_DIR"

echo "=============================="
echo "CIRRUS TEST PIPELINE"
echo "=============================="

############################################################
# Parse arguments
############################################################

CLEAN=0

while [[ $# -gt 0 ]]; do
    case "$1" in
        -c)
            CLEAN=1
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
    shift
done

############################################################
# Clean
############################################################

if [[ $CLEAN -eq 1 ]]; then
    if [[ -d build ]]; then
        echo "Removing build directory..."
        rm -rf build
    fi
fi

############################################################
# Configure
############################################################

mkdir -p build
cd build

if [[ ! -f CMakeCache.txt ]]; then
    cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..
fi

############################################################
# Build
############################################################

cmake --build . --target CIRRUS_tests

############################################################
# Run tests
############################################################

./CIRRUS_tests

echo
echo "=============================="
echo "ALL TESTS PASSED"
echo "=============================="