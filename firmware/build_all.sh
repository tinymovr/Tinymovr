#!/bin/bash

# Directory where final binaries will be placed
DEST_DIR="./release_binaries"

# Create the destination directory if it doesn't exist
mkdir -p "$DEST_DIR"

# Board revisions for which the firmware needs to be built
REVISIONS=("R32" "R33" "R50" "R51" "R52" "M51")

# Build types
TYPES=("release" "upgrade")

# Get the GIT version from makefile (assuming git is installed and the repository is properly initialized)
GIT_VERSION=$(git describe --abbrev=7 --dirty --always --tags)

# Loop through each revision and type to build and copy the firmware
for rev in "${REVISIONS[@]}"; do
    for type in "${TYPES[@]}"; do
        # Build the firmware
        make REV=$rev $type
        
        # Create the filename based on the given pattern
        FILENAME="tinymovr-$GIT_VERSION-$rev-$type.bin"
        
        # Copy the .bin file to the destination directory with the new filename
        cp "./build/tinymovr_fw.bin" "$DEST_DIR/$FILENAME"
        
        # Clean up build files to ensure fresh build for next iteration
        make clean
    done
done

echo "All binaries built and placed in $DEST_DIR."
