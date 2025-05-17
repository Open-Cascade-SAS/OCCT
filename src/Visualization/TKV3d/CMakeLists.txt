project(TKV3d)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKV3d "TKV3d_pch.hxx" TRUE)
