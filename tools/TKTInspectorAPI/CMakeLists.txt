project(TKTInspectorAPI)

SET (TOOL_API_LIBRARY ON)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_tool)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_tool)

UNSET (TOOL_API_LIBRARY)

