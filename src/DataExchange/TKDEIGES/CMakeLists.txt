project(TKDEIGES)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKDEIGES "${CMAKE_CURRENT_SOURCE_DIR}/TKDEIGES_pch.hxx" TRUE)
