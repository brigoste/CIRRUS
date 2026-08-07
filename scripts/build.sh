#!/usr/bin/env bash

set -e

############################################################
# Project configuration
############################################################

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

############################################################
# Defaults
############################################################

BUILD_TYPE="Release"
RUN=1
PLOT=0
LOG=0
QUIET=0
CLEAN=0
CLEAN_CACHE=0

CONFIG_PATH="$PROJECT_DIR/cases/verification/verification_suite.json"

############################################################
# Parse arguments
############################################################

while [[ $# -gt 0 ]]; do
    case "$1" in

        -c)
            CLEAN=1
            ;;

        -cc)
            CLEAN_CACHE=1
            ;;

        -i)
            CLEAN=0
            ;;

        -d)
            BUILD_TYPE="Debug"
            ;;

        -r)
            BUILD_TYPE="Release"
            ;;

        --run)
            RUN=1
            ;;

        --norun)
            RUN=0
            ;;

        --config)
            shift
            CONFIG_PATH="$1"
            ;;

        --plot)
            PLOT=1
            ;;

        --log)
            LOG=1
            ;;

        --quiet)
            QUIET=1
            ;;

        --help)

cat << EOF

===================== CIRRUS BUILD SYSTEM =====================

Usage:

./build.sh [options]

Build

    -c          Clean build
    -cc         Remove CMake cache
    -i          Incremental build

    -d          Debug

    -r          Release

Run

    --run
    --norun

    --config <file>

Other

    --plot

    --log

    --quiet

==============================================================

EOF

exit 0
;;

        *)
            echo "Unknown option: $1"
            exit 1
            ;;

    esac

    shift
done

############################################################
# Display configuration
############################################################

if [[ $QUIET -eq 0 ]]; then

echo "======================================"
echo "Build Type : $BUILD_TYPE"
echo "Config     : $CONFIG_PATH"
echo "======================================"

fi

############################################################
# Logging
############################################################

if [[ $LOG -eq 1 ]]; then

LOGFILE="$PROJECT_DIR/build.log"

echo "===== CIRRUS BUILD LOG =====" > "$LOGFILE"

fi

############################################################
# Build directory
############################################################

cd "$PROJECT_DIR"

if [[ $CLEAN -eq 1 ]]; then
    rm -rf build
fi

mkdir -p build

cd build

if [[ $CLEAN_CACHE -eq 1 ]]; then
    rm -f CMakeCache.txt
fi

############################################################
# Configure
############################################################

if [[ ! -f CMakeCache.txt ]]; then

cmake -G Ninja \
    -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
    ..

fi

############################################################
# Build
############################################################

if [[ $QUIET -eq 0 ]]; then
    echo "Building..."
fi

if [[ $LOG -eq 1 ]]; then
    cmake --build . >> "$LOGFILE" 2>&1
else
    cmake --build .
fi

############################################################
# Run
############################################################

if [[ $RUN -eq 1 ]]; then

if [[ -f CIRRUS ]]; then

./CIRRUS --config "$CONFIG_PATH"

else

echo "Executable not found."

fi

fi

############################################################
# Plot
############################################################

if [[ $PLOT -eq 1 ]]; then

python3 "$PROJECT_DIR/scripts/Plot.py"

fi

############################################################

echo
echo "Build complete."