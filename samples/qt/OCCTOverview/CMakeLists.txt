project(OCCTOverview)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)
include_directories("${CMAKE_SOURCE_DIR}/${RELATIVE_DIR}/../OCCTOverview/code")
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)

ADD_DEFINITIONS(-DNO_COMMONSAMPLE_EXPORTS -DNO_IESAMPLE_EXPORTS)
