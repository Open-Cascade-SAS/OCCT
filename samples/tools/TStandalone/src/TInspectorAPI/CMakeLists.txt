project(TInspectorAPI)

set (TOOLKIT_NAME ON)
set (INSTALL_API ON)

OCCT_INCLUDE_CMAKE_FILE_STANDALONE ("adm/cmake/occt_toolkit_standalone")

unset (TOOLKIT_NAME)
unset (INSTALL_API)

