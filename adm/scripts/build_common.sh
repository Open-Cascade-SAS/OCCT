#!/bin/bash

# Extract version info from version.cmake
anOcctVerSuffix=`grep -e "OCC_VERSION_DEVELOPMENT" "$aScriptDir/../cmake/version.cmake" | grep -o '".*"' | tr -d '"'`
OCC_VERSION_MAJOR=`grep -e "OCC_VERSION_MAJOR" "$aScriptDir/../cmake/version.cmake" | awk '{print $3}'`
OCC_VERSION_MINOR=`grep -e "OCC_VERSION_MINOR" "$aScriptDir/../cmake/version.cmake" | awk '{print $3}'`
OCC_VERSION_MAINTENANCE=`grep -e "OCC_VERSION_MAINTENANCE" "$aScriptDir/../cmake/version.cmake" | awk '{print $3}'`
anOcctVersion="$OCC_VERSION_MAJOR.$OCC_VERSION_MINOR.$OCC_VERSION_MAINTENANCE"
