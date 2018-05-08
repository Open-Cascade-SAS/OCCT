project(FuncDemo)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)

if (BUILD_Inspector)
  target_link_libraries (FuncDemo TKTInspector)
endif()
