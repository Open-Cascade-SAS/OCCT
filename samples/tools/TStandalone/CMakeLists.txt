cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)
project (TStandalone)

set (CASROOT "" CACHE PATH "Third party OpenCascade dir" )
set (3RDPARTY_DIR "" CACHE PATH "Third party dir" )
set (INSTALL_DIR "" CACHE PATH "Where to install" )

if (NOT "$ENV{CASROOT}" STREQUAL "" AND EXISTS "$ENV{CASROOT}")
  set (CASROOT "$ENV{CASROOT}")
  #set (3RDPARTY_OCCT_DIR "${CASROOT}")
  #message("Environment CASROOT: ${CASROOT}")
  #message("Environment 3RDPARTY_OCCT_DIR: ${CASROOT}")
endif()

if (NOT "$ENV{THIRDPARTY_DIR}" STREQUAL ""  AND EXISTS "$ENV{THIRDPARTY_DIR}")
  set (3RDPARTY_DIR "$ENV{THIRDPARTY_DIR}")
  message("Environment THIRDPARTY_DIR: ${3RDPARTY_DIR}")
endif()

if (NOT "$ENV{INSTALL_DIR}" STREQUAL "")
  set (INSTALL_DIR "$ENV{INSTALL_DIR}")
  message("Environment INSTALL_DIR: ${INSTALL_DIR}")
endif()

# Define the next variable to has defined 3rdParties DLL dir variables
set (BUILD_SHARED_LIBS ON)

set (CASROOT_SOURCE_FILES "${CMAKE_SOURCE_DIR}/../../.."  CACHE PATH "OCCT sources dir to find tools packages" )
if ("${CASROOT}" STREQUAL "")
  set (CASROOT "${CASROOT_SOURCE_FILES}/work/install"  CACHE PATH "OCCT sources dir to find tools packages" )
endif()

macro (OCCT_INCLUDE_CMAKE_FILE BEING_INCLUDED_FILE)
  include (${CASROOT_SOURCE_FILES}/${BEING_INCLUDED_FILE}.cmake)
endmacro()

macro (OCCT_INCLUDE_CMAKE_FILE_STANDALONE BEING_INCLUDED_FILE)
  include (${CMAKE_SOURCE_DIR}/${BEING_INCLUDED_FILE}.cmake)
endmacro()

set (ADDITIONAL_DLL_DIR "" CACHE PATH "Additional paths that should be included into PATH" )
if (NOT "$ENV{ADDITIONAL_DLL_DIR}" STREQUAL "")
  set (ADDITIONAL_DLL_DIR "$ENV{ADDITIONAL_DLL_DIR}")
  message("Environment ADDITIONAL_DLL_DIR: ${ADDITIONAL_DLL_DIR}")
endif()

set (CMAKE_CONFIGURATION_TYPES Release Debug RelWithDebInfo CACHE INTERNAL "" FORCE)

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/3rdparty_macro")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

OCCT_INCLUDE_CMAKE_FILE_STANDALONE("adm/cmake/occt")
if (OCCT_LIBRARY_NOT_FOUND)
  message (FATAL_ERROR "Could NOT find OCCT Library in : ${3RDPARTY_OCCT_LIBRARY_DIR}" )
endif()

OCCT_MAKE_OS_WITH_BITNESS()
OCCT_MAKE_COMPILER_SHORT_NAME()

set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )
set (INSTALL_API_DIR "${INSTALL_DIR}/api" CACHE PATH "" FORCE)
set (INSTALL_DIR_BIN "${OS_WITH_BIT}/${COMPILER}/bin" CACHE PATH "" FORCE)
set (INSTALL_DIR_LIB "${OS_WITH_BIT}/${COMPILER}/lib" CACHE PATH "" FORCE)
set (INSTALL_DIR_INCLUDE "inc" CACHE PATH "")

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freetype")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freeimage")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/gl2ps")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tbb")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/qt")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

#include (${CMAKE_SOURCE_DIR}/adm/cmake/3rdparty.cmake)

set (3RDPARTY_DLL_PATH "${USED_3RDPARTY_OCCT_DIRS};\
${USED_3RDPARTY_FREETYPE_DIR};\
${USED_3RDPARTY_FREEIMAGE_DIRS};\
${USED_3RDPARTY_GL2PS_DIRS};\
${3RDPARTY_QT_DIR}/bin;\
${3RDPARTY_TBB_DLL_DIR}")

if (ADDITIONAL_DLL_DIR)
  set (3RDPARTY_DLL_PATH "${3RDPARTY_DLL_PATH};\
${ADDITIONAL_DLL_DIR}")
endif()

set (3RDPARTY_DLL_DEB_PATH "${3RDPARTY_DLL_PATH}")


set (BUILD_TOOLKITS
  TKTInspectorAPI
  TKView
  TKTreeModel
  TKDFBrowser
  TKVInspector
  TKShapeView
  TKTInspector
  TKToolsDraw
)

string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
message (STATUS "\nInfo: \(${CURRENT_TIME}\) Start collecting all header files into ${CMAKE_BINARY_DIR}/inc ...")
# collect all the headers to <binary dir>/inc folder
COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_BINARY_DIR}" "${BUILD_TOOLKITS}" "${CASROOT_SOURCE_FILES}/tools" "${INSTALL_DIR_INCLUDE}/inspector")

OCCT_INCLUDE_CMAKE_FILE_STANDALONE ("adm/cmake/standalone_macros")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/qt_macro")

set (BUILD_PROJECTS
  TInspectorAPI
  View
  TreeModel
  DFBrowserPane
  DFBrowserPaneXDE
  DFBrowser
  ShapeView
  VInspector
  TInspector
  TInspectorEXE
  ToolsDraw
)
# include patched toolkit projects or original ones
foreach (PROJECT_ITEM ${BUILD_PROJECTS})
  OCCT_ADD_SUBDIRECTORY ("src/${PROJECT_ITEM}")
endforeach()
