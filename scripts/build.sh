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

CONFIG_PATH="$PROJECT_DIR/cases/user/User_default.json"

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

        --verification)
            CONFIG_PATH="$PROJECT_DIR/cases/verification/verification_suite.json"
            ;;

        --config)
            shift
            if [[ $# -eq 0 ]]; then
                echo "Error: --config requires a file path."
                exit 1
            fi
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
            cat <<EOF
===================== CIRRUS BUILD SYSTEM =====================

Usage:

./build.sh [options]

Build

-c                  Clean build
-cc                 Remove CMake cache
-i                  Incremental build

-d                  Debug
-r                  Release

Run

--run               Run after building
--norun             Build only
--verification      Run verification suite
--config <file>     Run specified configuration

Other

--plot              Generate plots after run
--log               Write build output to build.log
--quiet             Suppress build configuration output

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
# Validate configuration
############################################################

if [[ ! -f "$CONFIG_PATH" ]]; then
    echo "Configuration file not found:"
    echo "  $CONFIG_PATH"
    exit 1
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

case "$OSTYPE" in
    linux-gnu*)
        PYTHON_EXECUTABLE="python3"
        ;;
    msys*|cygwin*|win32*)
        PYTHON_EXECUTABLE="python"
        ;;
    *)
        PYTHON_EXECUTABLE="python3"
        ;;
esac

export CIRRUS_PYTHON_EXECUTABLE="$PYTHON_EXECUTABLE"

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
    if [[ -f "$PROJECT_DIR/build/CIRRUS" ]]; then
        cd "$PROJECT_DIR"

        RUN_ARGS=(--config "$CONFIG_PATH")

        if [[ $PLOT -eq 1 ]]; then
            RUN_ARGS+=(--plot)
        fi

        ./build/CIRRUS "${RUN_ARGS[@]}"
    else
        echo "Executable not found."
        exit 1
    fi
fi

echo
echo "Build complete."