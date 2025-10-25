project(OCCTOverview)

# Sample configuration
set (EXECUTABLE_PROJECT ON)
set (USE_QT ON)
set (RELATIVE_DIR "samples/qt")
set (TARGET_FOLDER "Samples")

include_directories("${OCCT_BINARY_DIR}/${INSTALL_DIR_INCLUDE}/${RELATIVE_DIR}")
include_directories("${OCCT_ROOT_DIR}/${RELATIVE_DIR}/Common/src")
include_directories("${OCCT_ROOT_DIR}/${RELATIVE_DIR}/Interface/src")
include_directories("${OCCT_ROOT_DIR}/${RELATIVE_DIR}/OCCTOverview/src")
include_directories("${OCCT_ROOT_DIR}/${RELATIVE_DIR}/OCCTOverview/code")

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

# Target-specific definitions
target_compile_definitions(OCCTOverview PRIVATE -DNO_COMMONSAMPLE_EXPORTS -DNO_IESAMPLE_EXPORTS)
