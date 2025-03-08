project(TKMesh)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKMesh "${CMAKE_CURRENT_SOURCE_DIR}/TKMesh_pch.hxx" TRUE)
