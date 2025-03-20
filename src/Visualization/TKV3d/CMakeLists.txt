project(TKV3d)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKV3d "${CMAKE_CURRENT_SOURCE_DIR}/TKV3d_pch.hxx" TRUE)
