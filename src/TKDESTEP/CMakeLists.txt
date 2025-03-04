set (OCCT_PRIVATE_STATIC_LIB TRUE)

# internal project which will be hidden
project(TKDESTEP)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKDESTEP "${CMAKE_CURRENT_SOURCE_DIR}/TKDESTEP_pch.hxx" TRUE)

FLEX_AND_BISON_TARGET_APPLY ("StepFile" src)
