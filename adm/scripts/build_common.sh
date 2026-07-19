#!/bin/bash

# Ensure script dir is defined
if [ -z "$aScriptDir" ]; then
    aScriptDir=$(dirname "$0")
fi

# Check if version file exists
versionFile="$aScriptDir/../cmake/version.cmake"
if [ ! -f "$versionFile" ]; then
    echo "Error: version.cmake not found"
    exit 1
fi

# Extract version info from version.cmake
OCC_VERSION_MAJOR=$(awk '/set.*OCC_VERSION_MAJOR/ {print $3}' "$versionFile")
OCC_VERSION_MINOR=$(awk '/set.*OCC_VERSION_MINOR/ {print $3}' "$versionFile")
OCC_VERSION_MAINTENANCE=$(awk '/set.*OCC_VERSION_MAINTENANCE/ {print $3}' "$versionFile")
anOcctVerSuffix=$(awk '/set.*OCC_VERSION_DEVELOPMENT/ {
    if (NF > 2) {
        gsub(/[)" ]/, "", $3)
        print $3
    } else {
        print ""
    }
}' "$versionFile")

# Combine version string
anOcctVersion="${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}.${OCC_VERSION_MAINTENANCE}"

