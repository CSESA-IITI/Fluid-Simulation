#!/bin/bash

# Check if config is provided
CONFIG=${1:-debug} # Default to Debug if no config provided

# Set directory based on config
case "$CONFIG" in
    debug)
        DIRECTORY="bin/Debug-linux-x86_64/FluidSim"
        ;;
    release)
        DIRECTORY="bin/Release-linux-x86_64/FluidSim"
        ;;
    *)
        echo "Error: Invalid config. Use 'debug' or 'release'"
        exit 1
        ;;
esac

FILENAME="FluidSim"

# Check if directory exists
if [ ! -d "$DIRECTORY" ]; then
    echo "Error: Directory $DIRECTORY does not exist"
    exit 1
fi

# Check if file exists
if [ ! -f "$DIRECTORY/$FILENAME" ]; then
    echo "Error: File $FILENAME does not exist in $DIRECTORY"
    exit 1
fi

# Check if file is executable
if [ -x "$DIRECTORY/$FILENAME" ]; then
    "./$DIRECTORY/$FILENAME"
else
    echo "Error: File $FILENAME is not executable"
    exit 1
fi
