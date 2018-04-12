cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

set (ADDITIONAL_INLCUDE_DIR_DESCR "Additional directory to be added in the projects include section")
if (NOT DEFINED ADDITIONAL_INLCUDE_DIR)
  set (ADDITIONAL_INLCUDE_DIR "" CACHE PATH ${ADDITIONAL_INLCUDE_DIR_DESCR})
  get_filename_component (ADDITIONAL_INLCUDE_DIR "${ADDITIONAL_INLCUDE_DIR}" ABSOLUTE)
else()
  file (TO_CMAKE_PATH "${ADDITIONAL_INLCUDE_DIR}" ADDITIONAL_INLCUDE_DIR)
  set (ADDITIONAL_INLCUDE_DIR "${ADDITIONAL_INLCUDE_DIR}" CACHE PATH "${ADDITIONAL_INLCUDE_DIR_DESCR}" FORCE)
endif()

set (ADDITIONAL_DLL_DIR_DESCR "Additional directories that should be included into PATH")
if (NOT DEFINED ADDITIONAL_DLL_DIR)
  set (ADDITIONAL_DLL_DIR "" CACHE PATH ${ADDITIONAL_DLL_DIR_DESCR})
  get_filename_component (ADDITIONAL_DLL_DIR "${ADDITIONAL_DLL_DIR}" ABSOLUTE)
else()
  file (TO_CMAKE_PATH "${ADDITIONAL_DLL_DIR}" ADDITIONAL_DLL_DIR)
  set (ADDITIONAL_DLL_DIR "${ADDITIONAL_DLL_DIR}" CACHE PATH "${ADDITIONAL_DLL_DIR_DESCR}" FORCE)
endif()

set (CURRENT_DIR "${CMAKE_SOURCE_DIR}")
set (CMAKE_SOURCE_DIR "${CMAKE_SOURCE_DIR}/.." CACHE INTERNAL "" FORCE)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/adm/cmake")

set (CMAKE_SUPPRESS_REGENERATION TRUE)

set (CMAKE_CONFIGURATION_TYPES Release Debug RelWithDebInfo CACHE INTERNAL "" FORCE)

# macro: include patched file if it exists
macro (OCCT_INCLUDE_CMAKE_FILE BEING_INCLUDED_FILE)
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/${BEING_INCLUDED_FILE}.cmake")
    include (${BUILD_PATCH}/${BEING_INCLUDED_FILE}.cmake)
  else()
    include (${CMAKE_SOURCE_DIR}/${BEING_INCLUDED_FILE}.cmake)
  endif()
endmacro()

macro (OCCT_INCLUDE_3RDPARTY_PRODUCT USE_PRODUCT PRODUCT_CONFIG_FILE)
  if (${USE_PRODUCT})
    OCCT_INCLUDE_CMAKE_FILE ("${PRODUCT_CONFIG_FILE}")
  endif (${USE_PRODUCT})
endmacro()

# include variable description
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/vardescr")

# set type of OCCT libraries
if (NOT BUILD_LIBRARY_TYPE)
  set (BUILD_LIBRARY_TYPE "Shared" CACHE STRING "${BUILD_LIBRARY_TYPE_DESCR}" FORCE)
  SET_PROPERTY(CACHE BUILD_LIBRARY_TYPE PROPERTY STRINGS Shared Static)
endif()

if ("${BUILD_LIBRARY_TYPE}" STREQUAL "Shared")
  set (BUILD_SHARED_LIBS ON)

  if (NOT DEFINED BUILD_SHARED_LIBRARY_NAME_POSTFIX)
    set (BUILD_SHARED_LIBRARY_NAME_POSTFIX "" CACHE STRING "${BUILD_SHARED_LIBRARY_NAME_POSTFIX_DESCR}" FORCE)
  endif()
else()
  unset (BUILD_SHARED_LIBS)
  unset (BUILD_SHARED_LIBRARY_NAME_POSTFIX)
endif()

# the name of the project
project (Inspector)

if (WIN32)
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
endif()

# include occt macros
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# Solution folder property
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# a single-configuration generator like the Makefile generator defines CMAKE_BUILD_TYPE variable
# check this variable and set if it's required
if (DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE) # single-configuration generator.
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

if (BUILD_WITH_DEBUG)
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:DEBUG>:OCCT_DEBUG>)
endif()

# install dir of the project
if (NOT DEFINED INSTALL_DIR)
  # set default install directory for Windows
  if (WIN32 AND NOT DEFINED CMAKE_INSTALL_PREFIX)
    set (CMAKE_INSTALL_PREFIX "C:/opencascade-${OCC_VERSION_STRING_EXT}/inspector")
  endif()
  set (INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "${INSTALL_DIR_DESCR}")
else()
  file (TO_CMAKE_PATH "${INSTALL_DIR}" INSTALL_DIR)
  set (INSTALL_DIR "${INSTALL_DIR}" CACHE PATH "${INSTALL_DIR_DESCR}" FORCE)
endif()

# choose a variant of the layout of the install paths
if (NOT INSTALL_DIR_LAYOUT)
  if (WIN32)
    set (INSTALL_DIR_LAYOUT "Windows" CACHE STRING "${INSTALL_DIR_LAYOUT_DESCR}" FORCE)
  else()
    set (INSTALL_DIR_LAYOUT "Unix" CACHE STRING "${INSTALL_DIR_LAYOUT_DESCR}" FORCE)
  endif()
  SET_PROPERTY(CACHE INSTALL_DIR_LAYOUT PROPERTY STRINGS Windows Unix)
endif()

# check INSTALL_DIR_LAYOUT changes and update INSTALL_DIR_* paths if necessary
if (NOT DEFINED INSTALL_DIR_LAYOUT_PREV)
  set (INSTALL_DIR_LAYOUT_PREV "${INSTALL_DIR_LAYOUT}" CACHE INTERNAL "" FORCE)
elseif (NOT "${INSTALL_DIR_LAYOUT_PREV}" STREQUAL "${INSTALL_DIR_LAYOUT}")
  set (INSTALL_DIR_LAYOUT_PREV "${INSTALL_DIR_LAYOUT}" CACHE INTERNAL "" FORCE)
  # The structure of install folder should be reset due to changed layout
  OCCT_CHECK_AND_UNSET_INSTALL_DIR_SUBDIRS ()

  # Unset INSTALL_DIR_WITH_VERSION on windows
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Windows")
    OCCT_CHECK_AND_UNSET (INSTALL_DIR_WITH_VERSION)
  else()
    if (NOT DEFINED INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_WITH_VERSION OFF CACHE BOOL "${INSTALL_DIR_WITH_VERSION_DESCR}")
    endif()
  endif()
endif()

# check CMAKE_INSTALL_PREFIX changes and update INSTALL_DIR if necessary
if (NOT DEFINED CMAKE_INSTALL_PREFIX_PREV)
  set (CMAKE_INSTALL_PREFIX_PREV "${CMAKE_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
elseif (NOT "${CMAKE_INSTALL_PREFIX_PREV}" STREQUAL "${CMAKE_INSTALL_PREFIX}")
  # CMAKE_INSTALL_PREFIX has been changed at previous step
  set (CMAKE_INSTALL_PREFIX_PREV "${CMAKE_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
  
  # INSTALL_DIR is required to be updated
  set (INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "${INSTALL_DIR_DESCR}" FORCE)
endif()

# check INSTALL_DIR changes and update CMAKE_INSTALL_PREFIX if necessary
if (NOT DEFINED INSTALL_DIR_PREV)
  set (INSTALL_DIR_PREV "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)
elseif (NOT "${INSTALL_DIR_PREV}" STREQUAL "${INSTALL_DIR}")
  # INSTALL_DIR has been changed at previous step
  set (INSTALL_DIR_PREV "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)

  # sync CMAKE_INSTALL_PREFIX with INSTALL_DIR 
  set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)

  # set CMAKE_INSTALL_PREFIX_PREV to avoid the reset of structure of the install folder 
  set (CMAKE_INSTALL_PREFIX_PREV "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)
endif()

if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
  if (NOT DEFINED INSTALL_DIR_WITH_VERSION_PREV)
    set (INSTALL_DIR_WITH_VERSION_PREV "${INSTALL_DIR_WITH_VERSION}" CACHE INTERNAL "" FORCE)
  elseif (NOT "${INSTALL_DIR_WITH_VERSION_PREV}" STREQUAL "${INSTALL_DIR_WITH_VERSION}")
    # INSTALL_DIR_WITH_VERSION has been changed at previous step
    set (INSTALL_DIR_WITH_VERSION_PREV "${INSTALL_DIR_WITH_VERSION}" CACHE INTERNAL "" FORCE)

    OCCT_CHECK_AND_UNSET_INSTALL_DIR_SUBDIRS ()
  endif()
endif()

# hide CMAKE_INSTALL_PREFIX from a user
set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)

set (BIN_LETTER "")
if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set (BIN_LETTER "d")
elseif ("${CMAKE_BUILD_TYPE}" STREQUAL "RelWithDebInfo")
  set (BIN_LETTER "i")
endif()

# Get all used variables: OS_WITH_BIT, COMPILER
OCCT_MAKE_OS_WITH_BITNESS()
OCCT_MAKE_COMPILER_SHORT_NAME()

# do not define INSTALL_DIR_BIN for win.
# Leave library structure for win: <prefix>/win64/vc10/bin(d)
if (NOT DEFINED INSTALL_DIR_BIN)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_BIN "bin" CACHE PATH "${INSTALL_DIR_BIN_DESCR}")
  else()
    set (INSTALL_DIR_BIN "${OS_WITH_BIT}/${COMPILER}/bin" CACHE PATH "${INSTALL_DIR_BIN_DESCR}")
  endif()
endif()

# define folder contaning all shell/batch scripts
if (NOT DEFINED INSTALL_DIR_SCRIPT)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_SCRIPT "${INSTALL_DIR_BIN}" CACHE PATH "${INSTALL_DIR_SCRIPT_DESCR}")
  else()
    set (INSTALL_DIR_SCRIPT "." CACHE PATH "${INSTALL_DIR_SCRIPT_DESCR}")
  endif()
endif()

# place the libraries to <prefix>/lib folder for unix and leave old structure for windows
if (NOT DEFINED INSTALL_DIR_LIB)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_LIB "lib" CACHE PATH "${INSTALL_DIR_LIB_DESCR}")
  else()
    set (INSTALL_DIR_LIB "${OS_WITH_BIT}/${COMPILER}/lib" CACHE PATH "${INSTALL_DIR_LIB_DESCR}")
  endif()
endif()

# OCCT headers: <prefix>/inc for windows,
#               <prefix>/include/opencascade-7.0.0 for unix
if (NOT DEFINED INSTALL_DIR_INCLUDE)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_INCLUDE "include/opencascade" CACHE PATH "${INSTALL_DIR_INCLUDE_DESCR}")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_INCLUDE "include/opencascade-${OCC_VERSION_STRING_EXT}" CACHE PATH "${INSTALL_DIR_INCLUDE_DESCR}" FORCE)
    endif()
  else()
    set (INSTALL_DIR_INCLUDE "inc" CACHE PATH "${INSTALL_DIR_INCLUDE_DESCR}")
  endif()
endif()

# OCCT resources: <prefix>/src for windows,
#                 <prefix>/share/opencascade-7.0.0/resources for unix
if (NOT DEFINED INSTALL_DIR_RESOURCE)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_RESOURCE "share/opencascade/resources" CACHE PATH "${INSTALL_DIR_RESOURCE_DESCR}")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_RESOURCE "share/opencascade-${OCC_VERSION_STRING_EXT}/resources" CACHE PATH "${INSTALL_DIR_RESOURCE_DESCR}" FORCE)
    endif()
  else()
    set (INSTALL_DIR_RESOURCE "src" CACHE PATH "${INSTALL_DIR_RESOURCE_DESCR}")
  endif()
endif()

# OCCT data
if (NOT DEFINED INSTALL_DIR_DATA)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_DATA "share/opencascade/data" CACHE PATH "${INSTALL_DIR_DATA_DESCR}")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_DATA "share/opencascade-${OCC_VERSION_STRING_EXT}/data" CACHE PATH "${INSTALL_DIR_DATA_DESCR}" FORCE)
    endif()
  else()
    set (INSTALL_DIR_DATA "data" CACHE PATH "${INSTALL_DIR_DATA_DESCR}")
  endif()
endif()

# include the patched or original list of definitions and flags
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_defs_flags")

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/3rdparty_macro")

if (NOT DEFINED 3RDPARTY_DIR)
  set (3RDPARTY_DIR "" CACHE PATH ${3RDPARTY_DIR_DESCR})
  get_filename_component (3RDPARTY_DIR "${3RDPARTY_DIR}" ABSOLUTE)
else()
  file (TO_CMAKE_PATH "${3RDPARTY_DIR}" 3RDPARTY_DIR)
  set (3RDPARTY_DIR "${3RDPARTY_DIR}" CACHE PATH "${3RDPARTY_DIR_DESCR}" FORCE)
endif()

find_package(OpenCASCADE QUIET PATHS "${OpenCASCADE_DIR}" "${OpenCASCADE_INSTALL_PREFIX}" NO_DEFAULT_PATH)
# include OCCT headers
include_directories ("${OpenCASCADE_INCLUDE_DIR}")

if (NOT OpenCASCADE_FOUND)
  message (FATAL_ERROR "coult not find OpenCASCADE, please set OpenCASCADE_DIR variable" )
else()
  set (OpenCASCADE_DIR "${OpenCASCADE_INSTALL_PREFIX}" CACHE PATH "OpenCASCADE directory" FORCE)
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_DIR}\"" )
endif()

set (USE_TCL "${OpenCASCADE_WITH_TCL}")
set (USE_FREETYPE "${OpenCASCADE_WITH_FREETYPE}")
set (USE_FREEIMAGE "${OpenCASCADE_WITH_FREEIMAGE}")
set (USE_GL2PS "${OpenCASCADE_WITH_GL2PS}")
set (USE_TBB "${OpenCASCADE_WITH_TBB}")
set (USE_VTK "${OpenCASCADE_WITH_VTK}")
set (USE_FFMPEG "${OpenCASCADE_WITH_FFMPEG}")
set (USE_GLES2 "${OpenCASCADE_WITH_GLES2}")
set (USE_GLX "${OpenCASCADE_WITH_GLX}")


# define CSF variable
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_csf")

# custom definitions (CMakeConfig of OCCT)
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_TCL "adm/cmake/tcl")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_FREETYPE "adm/cmake/freetype")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_FREEIMAGE "adm/cmake/freeimage")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_GL2PS "adm/cmake/gl2ps")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_TBB "adm/cmake/tbb")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_VTK "adm/cmake/vtk")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_FFMPEG "adm/cmake/ffmpeg")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_GLES2 "adm/cmake/egl")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_GLES2 "adm/cmake/gles2")
OCCT_INCLUDE_3RDPARTY_PRODUCT (USE_GLX "adm/cmake/glx")


OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/qt")
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

#include (${CMAKE_SOURCE_DIR}/adm/cmake/3rdparty.cmake)

set (3RDPARTY_DLL_DEB_PATH "${3RDPARTY_DLL_PATH}")

# build directories
if (SINGLE_GENERATOR)
  set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BIN_LETTER}")
  set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BIN_LETTER}")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BIN_LETTER}")
  if (WIN32)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BIN_LETTER}")
  endif()
endif()

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libi")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libi")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libd")
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/libd")

if (WIN32) 
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bini")
  set (CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bind")
endif()

# OCCT tools
# include the patched or original list of tools
# list <TOOLNAME>_TOOLKITS is created foreach tool and contains its toolkits
# list <OCCT_TOOLS> will contain all tools
#if (BUILD_Inspector)
  OCCT_MODULES_AND_TOOLKITS (TOOLS "TOOL_TOOLKITS" OCCT_TOOLS)
  foreach (OCCT_TOOL ${OCCT_TOOLS})
    list (APPEND BUILD_TOOL_TOOLKITS ${${OCCT_TOOL}_TOOL_TOOLKITS})
  endforeach()

  # collect all the headers to <binary dir>/inc/inspector folder
  string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
  message (STATUS "\nInfo: \(${CURRENT_TIME}\) Start collecting all OCCT tool header files into ${CMAKE_BINARY_DIR}/inc/inspector ...")
  COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_BINARY_DIR}" "${BUILD_TOOL_TOOLKITS}" "${CMAKE_SOURCE_DIR}/tools" "${INSTALL_DIR_INCLUDE}/inspector")
#endif()

# include <cmake binary folder>/inc
include_directories (${CMAKE_BINARY_DIR}/${INSTALL_DIR_INCLUDE})

include_directories (${ADDITIONAL_INLCUDE_DIR})

if (3RDPARTY_INCLUDE_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_INCLUDE_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_INCLUDE_DIRS_WITH_ENDS "${3RDPARTY_INCLUDE_DIRS}")
  message (STATUS "Info: The directories of 3rdparty headers: \n\t${3RDPARTY_INCLUDE_DIRS_WITH_ENDS}")
  include_directories (${3RDPARTY_INCLUDE_DIRS})
endif()

if (3RDPARTY_LIBRARY_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_LIBRARY_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_LIBRARY_DIRS_WITH_ENDS "${3RDPARTY_LIBRARY_DIRS}")
  message (STATUS "Info: The directories of 3rdparty libraries: \n\t${3RDPARTY_LIBRARY_DIRS_WITH_ENDS}")
  link_directories (${3RDPARTY_LIBRARY_DIRS})
endif()

# include patched toolkit projects or original ones
if (BUILD_TOOL_TOOLKITS)
  foreach (BUILD_TOOL_TOOLKIT ${BUILD_TOOL_TOOLKITS})
    OCCT_ADD_SUBDIRECTORY ("tools/${BUILD_TOOL_TOOLKIT}")
  endforeach()
endif()

message (STATUS "Info: \(${CURRENT_TIME}\) OCCT toolkits processed")

OCCT_MODULES_AND_TOOLKITS (SAMPLES "SAMPLES_TOOLKITS" OCCT_SAMPLES)

# patch TInspectorEXE
if (MSVC AND 3RDPARTY_DLL_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_DLL_DIRS)
  set (3RDPARTY_DLL_DIRS_FOR_PATH "")

  foreach (3RDPARTY_DLL_DIR ${3RDPARTY_DLL_DIRS})
    set (3RDPARTY_DLL_DIRS_FOR_PATH "${3RDPARTY_DLL_DIRS_FOR_PATH};${3RDPARTY_DLL_DIR}")
  endforeach()

  if (ADDITIONAL_DLL_DIR)
    foreach (3RDPARTY_DLL_DIR ${ADDITIONAL_DLL_DIR})
      set (3RDPARTY_DLL_DIRS_FOR_PATH "${3RDPARTY_DLL_DIRS_FOR_PATH};${3RDPARTY_DLL_DIR}")
    endforeach()
  endif (ADDITIONAL_DLL_DIR)

  OCCT_MAKE_COMPILER_BITNESS()
  set (X_COMPILER_BITNESS "x64")
  if ("${COMPILER_BITNESS}" STREQUAL "32")
    set (X_COMPILER_BITNESS "Win32")
  endif()

  # patch TInspectorEXE
  OCCT_CONFIGURE ("adm/templates/TInspectorEXE.vcxproj.user.in" "${CMAKE_BINARY_DIR}/TInspectorEXE/TInspectorEXE.vcxproj.user")
endif()

message (STATUS "Info: \(${CURRENT_TIME}\) Inspector configuration files prepared")
