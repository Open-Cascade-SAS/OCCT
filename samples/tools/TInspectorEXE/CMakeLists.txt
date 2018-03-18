project(TInspectorEXE)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)
set (RELATIVE_DIR "samples/tools")
include_directories("${CMAKE_SOURCE_DIR}/${RELATIVE_DIR}/TInspectorEXE/src")
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)
