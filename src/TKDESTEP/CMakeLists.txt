project(TKDESTEP)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

FLEX_AND_BISON_TARGET_APPLY ("StepFile" src)
