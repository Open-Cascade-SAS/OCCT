project(TKXSBase)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKXSBase "${CMAKE_CURRENT_SOURCE_DIR}/TKXSBase_pch.hxx" TRUE)
