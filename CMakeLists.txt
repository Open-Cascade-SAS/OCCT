cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

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

# include variable description
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/vardescr")

# set type of OCCT libraries
if (NOT BUILD_LIBRARY_TYPE)
  set (BUILD_LIBRARY_TYPE "Shared" CACHE STRING "${BUILD_LIBRARY_TYPE_DESCR}" FORCE)
  SET_PROPERTY(CACHE BUILD_LIBRARY_TYPE PROPERTY STRINGS Shared Static)
endif()

if ("${BUILD_LIBRARY_TYPE}" STREQUAL "Shared")
  set (BUILD_SHARED_LIBS ON)
else()
  unset (BUILD_SHARED_LIBS)
endif()

# the name of the project
project (OCCT)

# include occt macros
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# Solution folder property
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

# get current OCCT version
OCC_VERSION (OCC_VERSION_MAJOR OCC_VERSION_MINOR OCC_VERSION_MAINTENANCE OCC_VERSION_DEVELOPMENT OCC_VERSION_STRING_EXT)

set_property (GLOBAL PROPERTY OCC_VERSION_MAJOR ${OCC_VERSION_MAJOR})
set_property (GLOBAL PROPERTY OCC_VERSION_MINOR ${OCC_VERSION_MINOR})
set_property (GLOBAL PROPERTY OCC_VERSION_MAINTENANCE ${OCC_VERSION_MAINTENANCE})

set (INSTALL_TEST_CASES OFF CACHE BOOL "${INSTALL_TEST_CASES_DESCR}")

# single-configuration generator
set (SINGLE_GENERATOR OFF)
if (CMAKE_BUILD_TYPE)
  set (SINGLE_GENERATOR ON)
endif()

# a single-configuration generator like the Makefile generator defines CMAKE_BUILD_TYPE variable
# check this variable and set if it's required
if (DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE) # single-configuration generator.
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# enable extended messages of many OCCT algorithms
set (BUILD_WITH_DEBUG OFF CACHE BOOL "${BUILD_WITH_DEBUG_DESCR}")
if (BUILD_WITH_DEBUG)
  add_definitions (-DOCCT_DEBUG)
endif()

# copy samples to install directory
set (INSTALL_SAMPLES OFF CACHE BOOL "${INSTALL_SAMPLES_DESCR}")

# install dir of the project
if (NOT DEFINED INSTALL_DIR)
  set (INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "${INSTALL_DIR_DESCR}")
endif()

if (NOT DEFINED CMAKE_INSTALL_PREFIX_PREV)
  set (CMAKE_INSTALL_PREFIX_PREV "${CMAKE_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
elseif (NOT "${CMAKE_INSTALL_PREFIX_PREV}" STREQUAL "${CMAKE_INSTALL_PREFIX}")
  # CMAKE_INSTALL_PREFIX has been changed at previous step
  set (CMAKE_INSTALL_PREFIX_PREV "${CMAKE_INSTALL_PREFIX}" CACHE INTERNAL "" FORCE)
  
  # INSTALL_DIR is required to be updated
  set (INSTALL_DIR "${CMAKE_INSTALL_PREFIX}" CACHE PATH "${INSTALL_DIR_DESCR}" FORCE)
endif()

if (NOT DEFINED INSTALL_DIR_PREV)
  set (INSTALL_DIR_PREV "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)
elseif (NOT "${INSTALL_DIR_PREV}" STREQUAL "${INSTALL_DIR}")
  # INSTALL_DIR has been changed at previous step
  set (INSTALL_DIR_PREV "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)

  # sync CMAKE_INSTALL_PREFIX with INSTALL_DIR 
  set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)

  # set CMAKE_INSTALL_PREFIX_PREV to avoid the reset of structure of the install folder 
  set (CMAKE_INSTALL_PREFIX_PREV "${INSTALL_DIR}" CACHE INTERNAL "" FORCE)

  # The structure of install folder should be reset
  OCCT_CHECK_AND_UNSET (INSTALL_BIN_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_SCRIPT_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_LIB_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_INCLUDE_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_OCCT_RESOURCE_DIR)
  OCCT_CHECK_AND_UNSET (INSTALL_SHARE_DIR)
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

# do not define INSTALL_BIN_DIR for win. 
# Leave library structure for win: <prefix>/win64/vc10/bin(d)
if (NOT DEFINED INSTALL_BIN_DIR)
  if (UNIX)
    set (INSTALL_BIN_DIR "${INSTALL_DIR}/bin" CACHE PATH "${INSTALL_BIN_DIR_DESCR}")
  else()
    if (SINGLE_GENERATOR)
      set (INSTALL_BIN_DIR "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BIN_LETTER}" CACHE PATH "${INSTALL_BIN_DIR_DESCR}")
    endif()
  endif()
endif()

# define folder contaning all shell/batch scripts
if (NOT DEFINED INSTALL_SCRIPT_DIR)
  if (UNIX)
    set (INSTALL_SCRIPT_DIR "${INSTALL_BIN_DIR}/opencascade-${OCC_VERSION_STRING_EXT}" CACHE PATH "${INSTALL_SCRIPT_DIR_DESCR}")
  else()
    set (INSTALL_SCRIPT_DIR "${INSTALL_DIR}" CACHE PATH "${INSTALL_SCRIPT_DIR_DESCR}")
  endif()
endif()

# place the libraries to <prefix>/lib folder for unix and leave old structure for windows
if (NOT DEFINED INSTALL_LIB_DIR)
  if (UNIX)
    set (INSTALL_LIB_DIR "${INSTALL_DIR}/lib" CACHE PATH "${INSTALL_LIB_DIR_DESCR}")
  else()
    if (SINGLE_GENERATOR)
      set (INSTALL_LIB_DIR "${INSTALL_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BIN_LETTER}" CACHE PATH "${INSTALL_LIB_DIR_DESCR}")
    endif()
  endif()
endif()

# OCCT headers: <prefix>/inc for windows,
#               <prefix>/include/opencascade-7.0.0 for unix
if (NOT DEFINED INSTALL_INCLUDE_DIR)
  if (UNIX)
    set (INSTALL_INCLUDE_DIR "${INSTALL_DIR}/include/opencascade-${OCC_VERSION_STRING_EXT}" CACHE PATH "${INSTALL_INCLUDE_DIR_DESCR}")
  else()
    set (INSTALL_INCLUDE_DIR "${INSTALL_DIR}/inc" CACHE PATH "${INSTALL_INCLUDE_DIR_DESCR}")
  endif()
endif()

# OCCT resources: <prefix>/src for windows,
#                 <prefix>/share/opencascade-7.0.0/resources for unix
if (NOT DEFINED INSTALL_OCCT_RESOURCE_DIR)
  if (UNIX)
    set (INSTALL_OCCT_RESOURCE_DIR "${INSTALL_DIR}/share/opencascade-${OCC_VERSION_STRING_EXT}/resources" CACHE PATH "${INSTALL_OCCT_RESOURCE_DIR_DESCR}")
  else()
    set (INSTALL_OCCT_RESOURCE_DIR "${INSTALL_DIR}/src" CACHE PATH "${INSTALL_OCCT_RESOURCE_DIR_DESCR}")
  endif()
endif()

# OCCT data
if (NOT DEFINED INSTALL_SHARE_DIR)
  if (UNIX)
    set (INSTALL_SHARE_DIR "${INSTALL_DIR}/share/opencascade-${OCC_VERSION_STRING_EXT}" CACHE PATH "${INSTALL_SHARE_DIR_DESCR}")
  else()
    set (INSTALL_SHARE_DIR "${INSTALL_DIR}" CACHE PATH "${INSTALL_SHARE_DIR_DESCR}")
  endif()
endif()

# a directory recognized as a 'patch' for OCCT
set (BUILD_PATCH "" CACHE PATH "${BUILD_PATCH_DESCR}")

# the list of being built toolkits
set (BUILD_ADDITIONAL_TOOLKITS "" CACHE STRING "${BUILD_ADDITIONAL_TOOLKITS_DESCR}")
separate_arguments (BUILD_ADDITIONAL_TOOLKITS)

if (MSVC)
  set (BUILD_MODULE_OcctMfcSamples OFF CACHE BOOL "${BUILD_MODULE_OcctMfcSamples_DESCR}")
endif()

# whether use optional 3rdparty or not
if (APPLE)
  set (USE_GLX OFF CACHE BOOL "${USE_GLX_DESCR}")
endif()

if (WIN32)
  set (USE_D3D OFF CACHE BOOL "${USE_D3D_DESCR}")
endif()

# include the patched or original list of modules
# list <MODULENAME>_TOOLKITS is created foreach module and contains its toolkits
# list <OCCT_MODULES> will contain all modules
OCCT_MODULES_AND_TOOLKITS (OCCT_MODULES)

foreach (OCCT_MODULE ${OCCT_MODULES})
  BUILD_MODULE (${OCCT_MODULE})
endforeach()

if (ANDROID AND BUILD_MODULE_Draw)
  message (STATUS "Info. Draw module is turned off due to it is not supported on Android")
  set (BUILD_MODULE_Draw OFF CACHE BOOL "${BUILD_MODULE_Draw_DESCR}" FORCE)
endif()

# Overview
set (BUILD_DOC_OcctOverview OFF CACHE BOOL "${BUILD_DOC_OcctOverview_DESCR}")

if (NOT USE_D3D)
  list (REMOVE_ITEM Visualization_TOOLKITS TKD3DHost)
endif()

# accumulate used toolkits (first level) in BUILD_TOOLKITS variable
list (APPEND BUILD_TOOLKITS ${BUILD_ADDITIONAL_TOOLKITS})

foreach (OCCT_MODULE ${OCCT_MODULES})
  if (BUILD_MODULE_${OCCT_MODULE})
    list (APPEND BUILD_TOOLKITS ${${OCCT_MODULE}_TOOLKITS})
  endif()
endforeach()

# DRAWEXE excluded when library build is static
if (NOT BUILD_SHARED_LIBS)
  list (REMOVE_ITEM BUILD_TOOLKITS DRAWEXE)
  message (STATUS "Info: DRAWEXE is not included due to ${BUILD_LIBRARY_TYPE} build library type")
endif()

# accumulate all used toolkits
list (REMOVE_DUPLICATES BUILD_TOOLKITS)
set (RAW_BUILD_TOOLKITS)
foreach (BUILD_TOOLKIT ${BUILD_TOOLKITS})
  OCCT_TOOLKIT_FULL_DEP (${BUILD_TOOLKIT} TOOLKIT_FULL_DEPS)
  list (APPEND RAW_BUILD_TOOLKITS ${BUILD_TOOLKIT} ${TOOLKIT_FULL_DEPS})
endforeach()

list (REMOVE_DUPLICATES RAW_BUILD_TOOLKITS)
set (BUILD_TOOLKITS ${RAW_BUILD_TOOLKITS})

# include the patched or original list of definitions and flags
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_defs_flags")

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/3rdparty_macro")

# search for TKIVtk and TKIVtkDraw in BUILD_TOOLKITS
list (FIND BUILD_TOOLKITS TKIVtk CAN_USE_VTK)
if (CAN_USE_VTK EQUAL -1)
  list (FIND BUILD_TOOLKITS TKIVtkDraw CAN_USE_VTK)
endif()

if (NOT CAN_USE_VTK EQUAL -1)
  if (NOT DEFINED USE_VTK)
    set (USE_VTK OFF CACHE BOOL "${USE_VTK_DESCR}")
  endif()
else()
  OCCT_CHECK_AND_UNSET (USE_VTK)
endif()

# Rebuild *.yacc and *.lex files that are contained by TKMath toolkit
list (FIND BUILD_TOOLKITS TKMath   CAN_REBUILD_PDC_FOR_TKMATH)
list (FIND BUILD_TOOLKITS StepFile CAN_REBUILD_PDC_FOR_STEPFILE)

if (NOT ${CAN_REBUILD_PDC_FOR_TKMATH} EQUAL -1 OR NOT ${CAN_REBUILD_PDC_FOR_STEPFILE} EQUAL -1)
  set (BUILD_YACCLEX OFF CACHE BOOL "${BUILD_YACCLEX_DESCR}")
else()
  OCCT_CHECK_AND_UNSET (BUILD_YACCLEX)
endif()

if (NOT DEFINED 3RDPARTY_DIR)
  set (3RDPARTY_DIR "" CACHE PATH ${3RDPARTY_DIR_DESCR})
  get_filename_component (3RDPARTY_DIR "${3RDPARTY_DIR}" ABSOLUTE)
endif()

# search for CSF_TclLibs variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_TclLibs USE_TCL)

if (USE_TCL)
  message (STATUS "Info: TCL is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tcl")
  
  message (STATUS "Info: TK is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tk")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TCL")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TK")

  OCCT_CHECK_AND_UNSET ("INSTALL_TCL")
  OCCT_CHECK_AND_UNSET ("INSTALL_TK")
endif()

# search for CSF_FREETYPE variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_FREETYPE USE_FREETYPE)

if (USE_FREETYPE)
  message (STATUS "Info: Freetype is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freetype")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREETYPE")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build")
  OCCT_CHECK_AND_UNSET ("INSTALL_FREETYPE")
endif()

# VTK
if (USE_VTK)
  add_definitions (-DHAVE_VTK)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/vtk")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_VTK")
  
  if (NOT CAN_USE_VTK EQUAL -1)
    message (STATUS "Info: TKIVtk and TKIVtkDraw toolkits excluded due to VTK usage is disabled")

    list (REMOVE_ITEM BUILD_TOOLKITS TKIVtk)
    list (REMOVE_ITEM BUILD_TOOLKITS TKIVtkDraw)
  endif()
endif()

# D3D
if (USE_D3D)
  add_definitions (-DHAVE_D3D)
  #if(MSVC_VERSION LESS 1700)
  #OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/d3d")
  #endif()
endif()

# GLX
if (USE_GLX)
  add_definitions (-DMACOSX_USE_GLX)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/glx")
endif()

# FREEIMAGE
# search for CSF_FREEIMAGE variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_FREEIMAGE     CAN_USE_FREEIMAGE)
OCCT_IS_PRODUCT_REQUIRED (CSF_FreeImagePlus CAN_USE_FREEIMAGEPLUS)

if (CAN_USE_FREEIMAGE OR CAN_USE_FREEIMAGEPLUS)
  set (USE_FREEIMAGE OFF CACHE BOOL "${USE_FREEIMAGE_DESCR}")

  if (USE_FREEIMAGE)
    add_definitions (-DHAVE_FREEIMAGE)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freeimage")
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freeimageplus")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGE")
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGEPLUS")
    OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGE")
    OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGEPLUS")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_FREEIMAGE")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGE")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGEPLUS")
  OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGE")
  OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGEPLUS")
endif()

# GL2PS
# search for CSF_GL2PS variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_GL2PS CAN_USE_GL2PS)

if (NOT DEFINED ANDROID AND CAN_USE_GL2PS)
  set (USE_GL2PS OFF CACHE BOOL "${USE_GL2PS_DESCR}")

  if (USE_GL2PS)
    add_definitions (-DHAVE_GL2PS)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/gl2ps")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_GL2PS")
    OCCT_CHECK_AND_UNSET ("INSTALL_GL2PS")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_GL2PS")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_GL2PS")
  OCCT_CHECK_AND_UNSET ("INSTALL_GL2PS")
endif()

# TBB
# search for CSF_TBB variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_TBB CAN_USE_TBB)

if (NOT DEFINED ANDROID AND CAN_USE_TBB)
  set (USE_TBB OFF CACHE BOOL "${USE_TBB_DESCR}")

  if (USE_TBB)
    add_definitions (-DHAVE_TBB)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tbb")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TBB")
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TBBMALLOC")
    OCCT_CHECK_AND_UNSET ("INSTALL_TBB")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_TBB")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TBB")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TBBMALLOC")
  OCCT_CHECK_AND_UNSET ("INSTALL_TBB")
endif()

# Doxygen
if (BUILD_DOC_OcctOverview)
  if (NOT DEFINED INSTALL_DOC_OcctOverview)
    set (INSTALL_DOC_OcctOverview OFF CACHE BOOL "${INSTALL_DOC_OcctOverview_DESCR}")
  endif()

  if (INSTALL_DOC_OcctOverview)
    install (DIRECTORY "${CMAKE_BINARY_DIR}/doc/overview" DESTINATION  "${INSTALL_SHARE_DIR}/doc")

    # create overview.html only for windows
    if (WIN32)
      OCCT_INSTALL_FILE_OR_DIR ("dox/resources/overview.html" "${INSTALL_SCRIPT_DIR}")
    endif()
  endif()

  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/doxygen")
else()
  OCCT_CHECK_AND_UNSET ("INSTALL_DOC_OcctOverview")

  OCCT_CHECK_AND_UNSET ("3RDPARTY_DOXYGEN_EXECUTABLE")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_DOT_EXECUTABLE")
endif()

# bison 
if (BUILD_YACCLEX)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/bison")
else()
  OCCT_CHECK_AND_UNSET ("3RDPARTY_BISON_EXECUTABLE")
endif()

# flex 
if (BUILD_YACCLEX)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/flex")
else()
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FLEX_EXECUTABLE")
endif()

string (REGEX REPLACE ";" " " 3RDPARTY_NOT_INCLUDED "${3RDPARTY_NOT_INCLUDED}")

# check all 3rdparty paths
if (3RDPARTY_NOT_INCLUDED)
  message (FATAL_ERROR "NOT FOUND: ${3RDPARTY_NOT_INCLUDED}" )
endif()

if (3RDPARTY_INCLUDE_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_INCLUDE_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_INCLUDE_DIRS_WITH_ENDS "${3RDPARTY_INCLUDE_DIRS}")
  message (STATUS "Info: The directories of 3rdparty headers: ${3RDPARTY_INCLUDE_DIRS_WITH_ENDS}")
  include_directories (${3RDPARTY_INCLUDE_DIRS})
endif()

# include <cmake binary folder>/inc
include_directories (${CMAKE_BINARY_DIR}/inc)

if (3RDPARTY_LIBRARY_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_LIBRARY_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_LIBRARY_DIRS_WITH_ENDS "${3RDPARTY_LIBRARY_DIRS}")
  message (STATUS "Info: The directories of 3rdparty libraries: ${3RDPARTY_LIBRARY_DIRS_WITH_ENDS}")
  link_directories (${3RDPARTY_LIBRARY_DIRS})
endif()

# build directories
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

string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
message (STATUS "\nInfo: \(${CURRENT_TIME}\) Start collecting all OCCT header files into ${CMAKE_BINARY_DIR}/inc ...")

# collect all the headers to <binary dir>/inc folder
COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_BINARY_DIR}" "${BUILD_TOOLKITS}")

string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
message (STATUS "Info: \(${CURRENT_TIME}\) End the collecting")

OCCT_INSTALL_FILE_OR_DIR ("data" "${INSTALL_SHARE_DIR}")

# OCCT samples
if (INSTALL_SAMPLES)
  if (WIN32)
    OCCT_INSTALL_FILE_OR_DIR ("samples/CSharp" "${INSTALL_SHARE_DIR}/samples")
    OCCT_INSTALL_FILE_OR_DIR ("samples/mfc" "${INSTALL_SHARE_DIR}/samples")   
  endif()
  
  OCCT_INSTALL_FILE_OR_DIR ("samples/java" "${INSTALL_SHARE_DIR}/samples")
  OCCT_INSTALL_FILE_OR_DIR ("samples/ocafsamples" "${INSTALL_SHARE_DIR}/samples")
  OCCT_INSTALL_FILE_OR_DIR ("samples/qt" "${INSTALL_SHARE_DIR}/samples")
endif()

OCCT_INSTALL_FILE_OR_DIR ("samples/tcl" "${INSTALL_SHARE_DIR}/samples")

if (INSTALL_TEST_CASES)
  OCCT_INSTALL_FILE_OR_DIR ("tests" "${INSTALL_SHARE_DIR}")
endif()

if (WIN32)
  set (SCRIPT_EXT bat)
else()
  set (SCRIPT_EXT sh)
endif()

# copy draw script to install script folder
if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/adm/templates/draw.${SCRIPT_EXT}")
  install (FILES "${BUILD_PATCH}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_SCRIPT_DIR}"
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
else()
  install (FILES "${CMAKE_SOURCE_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_SCRIPT_DIR}"
          PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
endif()

# copy draw script to CMake binary folder
OCCT_COPY_FILE_OR_DIR ("adm/templates/draw.${SCRIPT_EXT}" "${CMAKE_BINARY_DIR}")

set (SUB_CUSTOM_NAME "custom_${COMPILER}_${COMPILER_BITNESS}.${SCRIPT_EXT}")

if (WIN32)
  set (ADDITIONAL_CUSTOM_CONTENT "\nif exist \"%~dp0${SUB_CUSTOM_NAME}\" (\n  call \"%~dp0${SUB_CUSTOM_NAME}\" %VCVER% %ARCH% %CASDEB% \n)")
else()
  set (ADDITIONAL_CUSTOM_CONTENT "\nif [ -e \"\${aScriptPath}/${SUB_CUSTOM_NAME}\" ]; then\n  source \"\${aScriptPath}/${SUB_CUSTOM_NAME}\" \"\$1\" \"\$2\" \nfi")
endif()

 # change custom.bat/sh
if (EXISTS "${INSTALL_BIN_DIR}/custom.${SCRIPT_EXT}")
  file (READ "${INSTALL_BIN_DIR}/custom.${SCRIPT_EXT}" CUSTOM_CONTENT)

  set (CUSTOM_CONTENT "${CUSTOM_CONTENT} ${ADDITIONAL_CUSTOM_CONTENT}")

  file (WRITE "${INSTALL_BIN_DIR}/custom.${SCRIPT_EXT}" "${CUSTOM_CONTENT}")
else()
  OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.${SCRIPT_EXT}.main" "custom.${SCRIPT_EXT}" "custom.${SCRIPT_EXT}" "${INSTALL_SCRIPT_DIR}")
endif()

# write current custom.bat/sh (for install directory)
set (SUB_CUSTOM_BUILD_NAME "custom_${COMPILER}_${COMPILER_BITNESS}.install.${SCRIPT_EXT}")
OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.install.${SCRIPT_EXT}.in" "${SUB_CUSTOM_BUILD_NAME}" "${SUB_CUSTOM_NAME}" "${INSTALL_SCRIPT_DIR}")

# write current custom.bat/sh (for build directory)
OCCT_CONFIGURE ("adm/templates/custom.build.${SCRIPT_EXT}.in" "${SUB_CUSTOM_NAME}")

if (BUILD_MODULE_OcctMfcSamples)
  OCCT_INSTALL_FILE_OR_DIR ("adm/templates/sample.bat" "${INSTALL_SCRIPT_DIR}")
  OCCT_COPY_FILE_OR_DIR    ("adm/templates/sample.bat" "${CMAKE_BINARY_DIR}")
endif()

# env script for draw in building environment
OCCT_CONFIGURE ("adm/templates/env.build.${SCRIPT_EXT}.in" "env.${SCRIPT_EXT}")

# end script for draw in standalone environment
if (UNIX)
  OCCT_CONFIGURE ("adm/templates/env.install.sh.in" "env.install.sh")
  install (FILES "${CMAKE_BINARY_DIR}/env.install.sh" DESTINATION  "${INSTALL_SCRIPT_DIR}" RENAME "env.sh")
else()
  # release version
  set (CURRENT_BUILD_LETTER "")
  OCCT_CONFIGURE ("adm/templates/env.install.bat.in" "env.install.release.bat")
  install (FILES "${CMAKE_BINARY_DIR}/env.install.release.bat" 
           CONFIGURATIONS Release
           DESTINATION  "${INSTALL_SCRIPT_DIR}" RENAME "env.bat")

  # debug version
  set (CURRENT_BUILD_LETTER "d")
  OCCT_CONFIGURE ("adm/templates/env.install.bat.in" "env.install.debug.bat")
  install (FILES "${CMAKE_BINARY_DIR}/env.install.debug.bat" 
           CONFIGURATIONS Debug
           DESTINATION  "${INSTALL_SCRIPT_DIR}" RENAME "env.bat")

  # release with debug info version
  set (CURRENT_BUILD_LETTER "i")
  OCCT_CONFIGURE ("adm/templates/env.install.bat.in" "env.install.relwithdebinfo.bat")
  install (FILES "${CMAKE_BINARY_DIR}/env.install.relwithdebinfo.bat" 
           CONFIGURATIONS RelWithDebInfo
           DESTINATION  "${INSTALL_SCRIPT_DIR}" RENAME "env.bat")
endif()

# copy DrawAppliInit from OCCT source to build directory
if (NOT EXISTS "${CMAKE_BINARY_DIR}/DrawAppliInit")
  OCCT_COPY_FILE_OR_DIR (DrawAppliInit "${CMAKE_BINARY_DIR}")
endif()

# RESOURCES
FILE_TO_LIST ("adm/RESOURCES" RESOURCES)
foreach(RESOURCE ${RESOURCES})
  get_filename_component(RESOURCE_FOLDER ${RESOURCE} DIRECTORY)
  if(NOT "${RESOURCE_FOLDER}" STREQUAL "")
    get_filename_component(RESOURCE_FOLDER ${RESOURCE_FOLDER} NAME)
    OCCT_INSTALL_FILE_OR_DIR ("src/${RESOURCE}" "${INSTALL_OCCT_RESOURCE_DIR}/${RESOURCE_FOLDER}")
  else()
    OCCT_INSTALL_FILE_OR_DIR ("src/${RESOURCE}" "${INSTALL_OCCT_RESOURCE_DIR}")
  endif()
endforeach()

# define CSF variable
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_csf")

# include patched toolkit projects or original ones
foreach (BUILD_TOOLKIT ${BUILD_TOOLKITS})
  OCCT_ADD_SUBDIRECTORY ("src/${BUILD_TOOLKIT}")
endforeach()

if (BUILD_DOC_OcctOverview)
  OCCT_ADD_SUBDIRECTORY (dox)
endif()

# patch DRAWEXE
if (MSVC AND 3RDPARTY_DLL_DIRS)
  list (FIND BUILD_TOOLKITS DRAWEXE DRAWEXE_INDEX)
  if (${DRAWEXE_INDEX} GREATER -1)
    list (REMOVE_DUPLICATES 3RDPARTY_DLL_DIRS)
    set (3RDPARTY_DLL_DIRS_FOR_PATH "")

    foreach (3RDPARTY_DLL_DIR ${3RDPARTY_DLL_DIRS})
      set (3RDPARTY_DLL_DIRS_FOR_PATH "${3RDPARTY_DLL_DIRS_FOR_PATH};${3RDPARTY_DLL_DIR}")
    endforeach()

    OCCT_MAKE_COMPILER_BITNESS()
    set (X_COMPILER_BITNESS "x64")
    if ("${COMPILER_BITNESS}" STREQUAL "32")
      set (X_COMPILER_BITNESS "Win32")
    endif()

    OCCT_CONFIGURE ("adm/templates/DRAWEXE.vcxproj.user.in" "${CMAKE_BINARY_DIR}/src/DRAWEXE/DRAWEXE.vcxproj.user")
  endif()
endif()

# samples do not support patch usage
if (BUILD_MODULE_OcctMfcSamples)
  set (OCCT_ROOT ${CMAKE_SOURCE_DIR})

  set (MFC_STANDARD_SAMPLES_DIR ${OCCT_ROOT}/samples/mfc/standard)
  set (COMMON_WINMAIN_FILE      ${MFC_STANDARD_SAMPLES_DIR}/Common/Winmain.cpp)

  add_subdirectory(samples/mfc/standard/mfcsample)
  add_subdirectory(samples/mfc/standard/01_Geometry)
  add_subdirectory(samples/mfc/standard/02_Modeling)
  add_subdirectory(samples/mfc/standard/03_Viewer2d)
  add_subdirectory(samples/mfc/standard/04_Viewer3d)
  add_subdirectory(samples/mfc/standard/05_ImportExport)
  add_subdirectory(samples/mfc/standard/06_Ocaf)
  add_subdirectory(samples/mfc/standard/07_Triangulation)
  add_subdirectory(samples/mfc/standard/08_HLR)
  add_subdirectory(samples/mfc/standard/09_Animation)
  add_subdirectory(samples/mfc/standard/10_Convert)
endif()
