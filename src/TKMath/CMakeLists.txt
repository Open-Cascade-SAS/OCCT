project(TKMath)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKMath "${CMAKE_CURRENT_SOURCE_DIR}/TKMath_pch.hxx" FALSE)
