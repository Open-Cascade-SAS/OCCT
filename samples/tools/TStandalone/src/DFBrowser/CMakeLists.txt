project(DFBrowser)

ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS)

set (USE_QT ON)
set (TOOLKIT_NAME ON)

OCCT_INCLUDE_CMAKE_FILE_STANDALONE ("adm/cmake/occt_toolkit_standalone")

unset (USE_QT)
unset (TOOLKIT_NAME)
