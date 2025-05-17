project(TKDESTEP)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Add the precompiled header
ADD_PRECOMPILED_HEADER(TKDESTEP "TKDESTEP_pch.hxx" TRUE)

FLEX_AND_BISON_TARGET_APPLY ("StepFile" src)
