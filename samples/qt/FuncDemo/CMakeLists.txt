project(FuncDemo)

# Sample configuration
set (EXECUTABLE_PROJECT ON)
set (USE_QT ON)
set (RELATIVE_DIR "samples/qt")
set (TARGET_FOLDER "Samples")

include_directories("${OCCT_BINARY_DIR}/${INSTALL_DIR_INCLUDE}/${RELATIVE_DIR}")

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

if (BUILD_Inspector)
  target_link_libraries (FuncDemo TKTInspector)
endif()
