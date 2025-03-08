project(TKBRep)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKBRep "${CMAKE_CURRENT_SOURCE_DIR}/TKBRep_pch.hxx" FALSE)
