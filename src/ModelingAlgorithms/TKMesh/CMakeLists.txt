project(TKMesh)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKMesh "TKMesh_pch.hxx" TRUE)
