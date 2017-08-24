cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

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
  message (STATUS "Info: Only 3rdparty's header files are used for building of static OCCT libraries")
  unset (BUILD_SHARED_LIBRARY_NAME_POSTFIX)
endif()

# the name of the project
project (OCCT)

if (WIN32)
  add_definitions(-DUNICODE)
  add_definitions(-D_UNICODE)
endif()

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

# Regeneration of OCCT resource files
set (BUILD_RESOURCES OFF CACHE BOOL "${BUILD_RESOURCES_DESCR}")

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
if (((SINGLE_GENERATOR AND "${CMAKE_BUILD_TYPE}" STREQUAL "Debug") OR NOT SINGLE_GENERATOR) AND (NOT "${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore"))
  if (NOT BUILD_WITH_DEBUG)
    set (BUILD_WITH_DEBUG OFF CACHE BOOL "${BUILD_WITH_DEBUG_DESCR}")
  endif()
else()
  OCCT_CHECK_AND_UNSET (BUILD_WITH_DEBUG)
endif()

if (BUILD_WITH_DEBUG)
  set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:DEBUG>:OCCT_DEBUG>)
endif()

# option to enable or disable use of precompiled headers
if (NOT DEFINED BUILD_USE_PCH)
  set (BUILD_USE_PCH OFF CACHE BOOL "${BUILD_USE_PCH_DESCR}")
endif()

if (BUILD_USE_PCH)

  # Load Cotire tool for accelerating build procedure
  include(cotire)

  # Set Cotire to ignore lxx, pxx, gxx 
  set (COTIRE_ADDITIONAL_PREFIX_HEADER_IGNORE_EXTENSIONS "lxx;pxx;gxx" CACHE STRING "Set Cotire to ignore OCCT specific files that can be #included" FORCE)

  # Set priority for inclusion of system headers in PCH to reduce problems
  # due to incomplete inclusion or wrong order.
  if (WIN32)
    # on Windows, assume that SDK (windows.h) is in default location
    set(ProgramFilesX86 "ProgramFiles(x86)")
    file(TO_CMAKE_PATH "$ENV{${ProgramFilesX86}}" ProgramFilesX86)
    set_property (DIRECTORY PROPERTY COTIRE_PREFIX_HEADER_INCLUDE_PRIORITY_PATH "${ProgramFilesX86}")
    unset(ProgramFilesX86)
  endif()

endif()

# copy samples to install directory
set (INSTALL_SAMPLES OFF CACHE BOOL "${INSTALL_SAMPLES_DESCR}")

# install dir of the project
if (NOT DEFINED INSTALL_DIR)
  # set default install directory for Windows
  if (WIN32 AND NOT DEFINED CMAKE_INSTALL_PREFIX)
    set (CMAKE_INSTALL_PREFIX "C:/opencascade-${OCC_VERSION_STRING_EXT}")
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

# OCCT samples
if (NOT DEFINED INSTALL_DIR_SAMPLES)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_SAMPLES "share/opencascade/samples" CACHE PATH "${INSTALL_DIR_SAMPLES_DESCR}")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_SAMPLES "share/opencascade-${OCC_VERSION_STRING_EXT}/samples" CACHE PATH "${INSTALL_DIR_SAMPLES_DESCR}" FORCE)
    endif()
  else()
    set (INSTALL_DIR_SAMPLES "samples" CACHE PATH "${INSTALL_DIR_SAMPLES_DESCR}")
  endif()
endif()

# OCCT tests
if (NOT DEFINED INSTALL_DIR_TESTS)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_TESTS "share/opencascade/tests" CACHE PATH "${INSTALL_DIR_TESTS_DESCR}")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_TESTS "share/opencascade-${OCC_VERSION_STRING_EXT}/tests" CACHE PATH "${INSTALL_DIR_TESTS_DESCR}" FORCE)
    endif()
  else()
    set (INSTALL_DIR_TESTS "tests" CACHE PATH "${INSTALL_DIR_TESTS_DESCR}")
  endif()
endif()

# OCCT doc
if (NOT DEFINED INSTALL_DIR_DOC)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    set (INSTALL_DIR_DOC "share/doc/opencascade" CACHE PATH "${INSTALL_DIR_DOC_DESCR}")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_DOC "share/doc/opencascade-${OCC_VERSION_STRING_EXT}" CACHE PATH "${INSTALL_DIR_DOC_DESCR}" FORCE)
    endif()
  else()
    set (INSTALL_DIR_DOC "doc" CACHE PATH "${INSTALL_DIR_DOC_DESCR}")
  endif()
endif()

# define folder contaning CMake configuration files
if (NOT DEFINED INSTALL_DIR_CMAKE)
  if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
    if (INSTALL_DIR_WITH_VERSION)
      set (INSTALL_DIR_CMAKE "lib/cmake/opencascade-${OCC_VERSION_STRING_EXT}" CACHE PATH "${INSTALL_DIR_CMAKE_DESCR}")
    else()
      set (INSTALL_DIR_CMAKE "lib/cmake/opencascade" CACHE PATH "${INSTALL_DIR_CMAKE_DESCR}")
    endif()
  else()
    set (INSTALL_DIR_CMAKE "cmake" CACHE PATH "${INSTALL_DIR_CMAKE_DESCR}")
  endif()
endif()

# include occt macros
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_resources")

# install LICENSE_LGPL_21.txt and OCCT_LGPL_EXCEPTION.txt files
if ("${INSTALL_DIR_LAYOUT}" STREQUAL "Unix")
  OCCT_INSTALL_FILE_OR_DIR ("LICENSE_LGPL_21.txt" "${INSTALL_DIR_DOC}")
  OCCT_INSTALL_FILE_OR_DIR ("OCCT_LGPL_EXCEPTION.txt" "${INSTALL_DIR_DOC}")
else()
  OCCT_INSTALL_FILE_OR_DIR ("LICENSE_LGPL_21.txt" ".")
  OCCT_INSTALL_FILE_OR_DIR ("OCCT_LGPL_EXCEPTION.txt" ".")
endif()

if(APPLE)
  set (INSTALL_NAME_DIR "" CACHE STRING "install_name library suffix on OS X (e.g. @executable_path/../Frameworks)")
endif()

# a directory recognized as a 'patch' for OCCT
set (BUILD_PATCH "" CACHE PATH "${BUILD_PATCH_DESCR}")

# the list of being built toolkits
set (BUILD_ADDITIONAL_TOOLKITS "" CACHE STRING "${BUILD_ADDITIONAL_TOOLKITS_DESCR}")
separate_arguments (BUILD_ADDITIONAL_TOOLKITS)

if (MSVC)
  set (BUILD_MODULE_MfcSamples OFF CACHE BOOL "${BUILD_MODULE_MfcSamples_DESCR}")
endif()

set (BUILD_Inspector OFF CACHE BOOL "${BUILD_Inspector_DESCR}")

# uwp sample
if (MSVC)
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
    set (BUILD_MODULE_UwpSample OFF CACHE BOOL "${BUILD_MODULE_UwpSample_DESCR}")
  else()
    unset (BUILD_MODULE_UwpSample)
  endif()
endif()

# whether use optional 3rdparty or not
if (APPLE)
  set (USE_GLX OFF CACHE BOOL "${USE_GLX_DESCR}")
endif()

if (WIN32)
  set (USE_D3D OFF CACHE BOOL "${USE_D3D_DESCR}")
endif()

# Enable/Disable the floating point exceptions (FPE) during runtime.
if (NOT BUILD_ENABLE_FPE_SIGNAL_HANDLER)
  set (BUILD_ENABLE_FPE_SIGNAL_HANDLER OFF CACHE BOOL "${BUILD_ENABLE_FPE_SIGNAL_HANDLER_DESCR}" FORCE)
endif()

if (BUILD_ENABLE_FPE_SIGNAL_HANDLER)
  set (BUILD_ENABLE_FPE_SIGNAL_HANDLER 1)
else()
  set (BUILD_ENABLE_FPE_SIGNAL_HANDLER 0)
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
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore" AND BUILD_MODULE_Draw)
  message (STATUS "Info. Draw module is turned off due to it is not supported on UWP")
  set (BUILD_MODULE_Draw OFF CACHE BOOL "${BUILD_MODULE_Draw_DESCR}" FORCE)
endif()

# Overview
if (NOT DEFINED BUILD_DOC_Overview)
  set (DO_ONLY_CHECK_FOR_DOXYGEN ON)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/doxygen")
  set (DO_ONLY_CHECK_FOR_DOXYGEN OFF)
  
  if (CAN_DOXYGEN_BE_USED)
    message (STATUS "Info. Overview building is turned on")
  endif()

  set (BUILD_DOC_Overview ${CAN_DOXYGEN_BE_USED} CACHE BOOL "${BUILD_DOC_Overview_DESCR}")
endif()

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
else()
  file (TO_CMAKE_PATH "${3RDPARTY_DIR}" 3RDPARTY_DIR)
  set (3RDPARTY_DIR "${3RDPARTY_DIR}" CACHE PATH "${3RDPARTY_DIR_DESCR}" FORCE)
endif()

# search for CSF variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_FreeImagePlus CAN_USE_FREEIMAGE)
OCCT_IS_PRODUCT_REQUIRED (CSF_TclLibs USE_TCL)
OCCT_IS_PRODUCT_REQUIRED (CSF_FREETYPE USE_FREETYPE)
OCCT_IS_PRODUCT_REQUIRED (CSF_OpenGlLibs CAN_USE_GLES2)
OCCT_IS_PRODUCT_REQUIRED (CSF_GL2PS CAN_USE_GL2PS)
OCCT_IS_PRODUCT_REQUIRED (CSF_TBB CAN_USE_TBB)
OCCT_IS_PRODUCT_REQUIRED (CSF_EIGEN CAN_USE_EIGEN)

# define CSF variable
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_csf")

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
  set (OCCT_VTK_USED_TARGETS "" CACHE INTERNAL "" FORCE)
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

if (CAN_USE_FREEIMAGE)
  set (USE_FREEIMAGE OFF CACHE BOOL "${USE_FREEIMAGE_DESCR}")

  if (USE_FREEIMAGE)
    add_definitions (-DHAVE_FREEIMAGE)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freeimage")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGE")
    OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGE")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_FREEIMAGE")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGE")
  OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGE")
endif()

# FFmpeg
# search for CSF_FFmpeg variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_FFmpeg CAN_USE_FFMPEG)

if (CAN_USE_FFMPEG)
  set (USE_FFMPEG OFF CACHE BOOL "${USE_FFMPEG_DESCR}")

  if (USE_FFMPEG)
    add_definitions (-DHAVE_FFMPEG)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/ffmpeg")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FFMPEG")
    OCCT_CHECK_AND_UNSET ("INSTALL_FFMPEG")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_FFMPEG")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FFMPEG")
  OCCT_CHECK_AND_UNSET ("INSTALL_FFMPEG")
endif()

# OpenGL ES 2.0
if (WIN32 AND CAN_USE_GLES2)
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
    set (USE_GLES2 ON)
  else()
    set (USE_GLES2 OFF CACHE BOOL "${USE_GLES2_DESCR}")
  endif()

  if (USE_GLES2)
    add_definitions (-DHAVE_GLES2)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/egl")
	OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/gles2")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_EGL")
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_GLES2")
    OCCT_CHECK_AND_UNSET ("INSTALL_EGL")
    OCCT_CHECK_AND_UNSET ("INSTALL_GLES2")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_GLES2")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_EGL")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_GLES2")
  OCCT_CHECK_AND_UNSET ("INSTALL_EGL")
  OCCT_CHECK_AND_UNSET ("INSTALL_GLES2")
endif()

# GL2PS
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

# EIGEN
if (CAN_USE_EIGEN)
  set (USE_EIGEN OFF CACHE BOOL "${USE_EIGEN_DESCR}")

  if (USE_EIGEN)
    add_definitions (-DHAVE_EIGEN)
    OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/eigen")
  else()
    OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_EIGEN")
    OCCT_CHECK_AND_UNSET ("INSTALL_EIGEN")
  endif()
else()
  OCCT_CHECK_AND_UNSET ("USE_EIGEN")

  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_EIGEN")
  OCCT_CHECK_AND_UNSET ("INSTALL_EIGEN")
endif()

# Doxygen
if (BUILD_DOC_Overview)
  if (NOT DEFINED INSTALL_DOC_Overview)
    set (INSTALL_DOC_Overview OFF CACHE BOOL "${INSTALL_DOC_Overview_DESCR}")
  endif()

  if (INSTALL_DOC_Overview)
    install (DIRECTORY "${CMAKE_BINARY_DIR}/doc/overview" DESTINATION  "${INSTALL_DIR_DOC}")

    # create overview.html only for windows
    if (WIN32 AND "${INSTALL_DIR_LAYOUT}" STREQUAL "Windows")
      OCCT_INSTALL_FILE_OR_DIR ("dox/resources/overview.html" "${INSTALL_DIR_DOC}/..")
    endif()
  endif()

  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/doxygen")
else()
  OCCT_CHECK_AND_UNSET ("INSTALL_DOC_Overview")

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
  message (STATUS "Info: The directories of 3rdparty headers: \n\t${3RDPARTY_INCLUDE_DIRS_WITH_ENDS}")
  include_directories (${3RDPARTY_INCLUDE_DIRS})
endif()

# include <cmake binary folder>/inc
include_directories (${CMAKE_BINARY_DIR}/${INSTALL_DIR_INCLUDE})

if (3RDPARTY_LIBRARY_DIRS AND BUILD_SHARED_LIBS)
  list (REMOVE_DUPLICATES 3RDPARTY_LIBRARY_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_LIBRARY_DIRS_WITH_ENDS "${3RDPARTY_LIBRARY_DIRS}")
  message (STATUS "Info: The directories of 3rdparty libraries: \n\t${3RDPARTY_LIBRARY_DIRS_WITH_ENDS}")
  link_directories (${3RDPARTY_LIBRARY_DIRS})
endif()

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

string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
message (STATUS "\nInfo: \(${CURRENT_TIME}\) Start collecting all OCCT header files into ${CMAKE_BINARY_DIR}/inc ...")

# collect all the headers to <binary dir>/inc folder
COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_BINARY_DIR}" "${BUILD_TOOLKITS}" "${CMAKE_SOURCE_DIR}/src" "${INSTALL_DIR_INCLUDE}")

string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
message (STATUS "Info: \(${CURRENT_TIME}\) End the collecting")

list (FIND BUILD_TOOLKITS DRAWEXE DRAWEXE_INDEX)
if (${DRAWEXE_INDEX} GREATER -1)
  OCCT_INSTALL_FILE_OR_DIR ("data/" "${INSTALL_DIR_DATA}")
  OCCT_INSTALL_FILE_OR_DIR ("samples/tcl" "${INSTALL_DIR_SAMPLES}")
endif()

if (WIN32)
  set (SCRIPT_EXT bat)
else()
  set (SCRIPT_EXT sh)
endif()

# OCCT tools
# include the patched or original list of tools
# list <TOOLNAME>_TOOLKITS is created foreach tool and contains its toolkits
# list <OCCT_TOOLS> will contain all tools
if (NOT BUILD_Inspector)
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_QT")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TQTMALLOC")
  OCCT_CHECK_AND_UNSET ("INSTALL_QT")
else()
  OCCT_TOOLS_AND_TOOLKITS (OCCT_TOOLS)
  foreach (OCCT_TOOL ${OCCT_TOOLS})
    list (APPEND BUILD_TOOL_TOOLKITS ${${OCCT_TOOL}_TOOL_TOOLKITS})
  endforeach()

  # collect all the headers to <binary dir>/inc folder
  string(TIMESTAMP CURRENT_TIME "%H:%M:%S")
  message (STATUS "\nInfo: \(${CURRENT_TIME}\) Start collecting all OCCT tool header files into ${CMAKE_BINARY_DIR}/tools/inc ...")
  COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_BINARY_DIR}" "${BUILD_TOOL_TOOLKITS}" "${CMAKE_SOURCE_DIR}/tools" "${INSTALL_DIR_INCLUDE}/inspector")


  # check qt 3rdparty path
  add_definitions (-DHAVE_QT)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/qt")
  message (STATUS "Info: Qt is used by OCCT")
endif()

# OCCT samples
# get absolute path from INSTALL_DIR
set (INSTALL_DIR_ABSOLUTE "${INSTALL_DIR}")
if(NOT IS_ABSOLUTE "${INSTALL_DIR_ABSOLUTE}")
  get_filename_component(INSTALL_DIR_ABSOLUTE "${CMAKE_BINARY_DIR}/${INSTALL_DIR}" ABSOLUTE)
endif()

if (INSTALL_SAMPLES)
  OCCT_CONFIGURE ("adm/templates/env.samples.${SCRIPT_EXT}.in" "env.samples.${SCRIPT_EXT}")
  if (WIN32)
    OCCT_INSTALL_FILE_OR_DIR ("samples/CSharp" "${INSTALL_DIR_SAMPLES}")
    OCCT_INSTALL_FILE_OR_DIR ("samples/mfc" "${INSTALL_DIR_SAMPLES}")

    if ("${CMAKE_SYSTEM_NAME}" STREQUAL "WindowsStore")
      OCCT_INSTALL_FILE_OR_DIR ("samples/xaml" "${INSTALL_DIR_SAMPLES}")
    endif()

    install (FILES "${CMAKE_BINARY_DIR}/env.samples.${SCRIPT_EXT}" DESTINATION "${INSTALL_DIR_SAMPLES}/CSharp" RENAME "env.${SCRIPT_EXT}")
    install (FILES "${CMAKE_BINARY_DIR}/env.samples.${SCRIPT_EXT}" DESTINATION "${INSTALL_DIR_SAMPLES}/mfc/standard" RENAME "env.${SCRIPT_EXT}")
  endif()

  OCCT_INSTALL_FILE_OR_DIR ("samples/java" "${INSTALL_DIR_SAMPLES}")
  OCCT_INSTALL_FILE_OR_DIR ("samples/ocafsamples" "${INSTALL_DIR_SAMPLES}")
  OCCT_INSTALL_FILE_OR_DIR ("samples/qt" "${INSTALL_DIR_SAMPLES}")

  install (FILES "${CMAKE_BINARY_DIR}/env.samples.${SCRIPT_EXT}" DESTINATION "${INSTALL_DIR_SAMPLES}/qt/FuncDemo" RENAME "env.${SCRIPT_EXT}")
  install (FILES "${CMAKE_BINARY_DIR}/env.samples.${SCRIPT_EXT}" DESTINATION "${INSTALL_DIR_SAMPLES}/qt/IESample" RENAME "env.${SCRIPT_EXT}")
  install (FILES "${CMAKE_BINARY_DIR}/env.samples.${SCRIPT_EXT}" DESTINATION "${INSTALL_DIR_SAMPLES}/qt/Tutorial" RENAME "env.${SCRIPT_EXT}")
endif()

if (INSTALL_TEST_CASES)
  OCCT_INSTALL_FILE_OR_DIR ("tests/" "${INSTALL_DIR_TESTS}")
endif()

list (FIND BUILD_TOOLKITS DRAWEXE DRAWEXE_INDEX)
if (${DRAWEXE_INDEX} GREATER -1)
  # copy data and samples/tcl folders to install script folder
  OCCT_INSTALL_FILE_OR_DIR ("data/" "${INSTALL_DIR_DATA}")
  OCCT_INSTALL_FILE_OR_DIR ("samples/tcl" "${INSTALL_DIR_SAMPLES}")

  # copy draw script to install script folder
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/adm/templates/draw.${SCRIPT_EXT}")
    install (FILES "${BUILD_PATCH}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR_SCRIPT}"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  else()
    install (FILES "${CMAKE_SOURCE_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR_SCRIPT}"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  endif()

  # copy draw script to CMake binary folder
  OCCT_COPY_FILE_OR_DIR ("adm/templates/draw.${SCRIPT_EXT}" "${CMAKE_BINARY_DIR}")
endif()

set (SUB_CUSTOM_NAME "custom_${COMPILER}_${COMPILER_BITNESS}.${SCRIPT_EXT}")

if (WIN32)
  set (ADDITIONAL_CUSTOM_CONTENT "\nif exist \"%~dp0${SUB_CUSTOM_NAME}\" (\n  call \"%~dp0${SUB_CUSTOM_NAME}\" %1 %2 %3 \n)")
else()
  set (ADDITIONAL_CUSTOM_CONTENT "\nif [ -e \"\${aScriptPath}/${SUB_CUSTOM_NAME}\" ]; then\n  source \"\${aScriptPath}/${SUB_CUSTOM_NAME}\" \"\$1\" \"\$2\" \nfi")
endif()

 # change custom.bat/sh
if (EXISTS "${INSTALL_DIR}/${INSTALL_DIR_SCRIPT}/custom.${SCRIPT_EXT}")
  file (READ "${INSTALL_DIR}/${INSTALL_DIR_SCRIPT}/custom.${SCRIPT_EXT}" CUSTOM_CONTENT)

  set (CUSTOM_CONTENT "${CUSTOM_CONTENT} ${ADDITIONAL_CUSTOM_CONTENT}")

  file (WRITE "${INSTALL_DIR}/${INSTALL_DIR_SCRIPT}/custom.${SCRIPT_EXT}" "${CUSTOM_CONTENT}")
else()
  OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.${SCRIPT_EXT}.main" "custom.${SCRIPT_EXT}" "custom.${SCRIPT_EXT}" "${INSTALL_DIR_SCRIPT}")
endif()

if (WIN32)
  set (THIRDPARTY_DIR_REPLACE "%THIRDPARTY_DIR%")
else()
  set (THIRDPARTY_DIR_REPLACE "\${THIRDPARTY_DIR}")
endif()
get_cmake_property(USED_3RDPARTY_CACHE_VARIABLES VARIABLES)
string (REGEX MATCHALL "(^|;)USED_3RDPARTY_[^;]+_DIR[^;]*" USED_3RDPARTY_CACHE_VARIABLES "${USED_3RDPARTY_CACHE_VARIABLES}")
file (TO_CMAKE_PATH "${3RDPARTY_DIR}" 3RDPARTY_DIR)
foreach (USED_3RDPARTY_CACHE_VARIABLE ${USED_3RDPARTY_CACHE_VARIABLES})
   file (TO_CMAKE_PATH "${${USED_3RDPARTY_CACHE_VARIABLE}}" ${USED_3RDPARTY_CACHE_VARIABLE})
   string (REPLACE "${3RDPARTY_DIR}" "${THIRDPARTY_DIR_REPLACE}" ${USED_3RDPARTY_CACHE_VARIABLE} "${${USED_3RDPARTY_CACHE_VARIABLE}}")
   if (NOT WIN32)
     string (REGEX REPLACE ";" ":" ${USED_3RDPARTY_CACHE_VARIABLE} "${${USED_3RDPARTY_CACHE_VARIABLE}}")
   endif()
endforeach()

# write current custom.bat/sh (for install directory)
set (SUB_CUSTOM_BUILD_NAME "custom_${COMPILER}_${COMPILER_BITNESS}.install.${SCRIPT_EXT}")
OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.install.${SCRIPT_EXT}.in" "${SUB_CUSTOM_BUILD_NAME}" "${SUB_CUSTOM_NAME}" "${INSTALL_DIR_SCRIPT}")

# write current custom.bat/sh (for build directory)
OCCT_CONFIGURE ("adm/templates/custom.build.${SCRIPT_EXT}.in" "${SUB_CUSTOM_NAME}")

if (BUILD_MODULE_MfcSamples)
  OCCT_INSTALL_FILE_OR_DIR ("adm/templates/sample.bat" "${INSTALL_DIR_SCRIPT}")
  OCCT_COPY_FILE_OR_DIR    ("adm/templates/sample.bat" "${CMAKE_BINARY_DIR}")
endif()

# env script for draw in building environment
OCCT_CONFIGURE ("adm/templates/env.${SCRIPT_EXT}.in" "env.${SCRIPT_EXT}")

# install env script
install (FILES "${CMAKE_BINARY_DIR}/env.${SCRIPT_EXT}" DESTINATION "${INSTALL_DIR_SCRIPT}")

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
    OCCT_INSTALL_FILE_OR_DIR ("src/${RESOURCE}" "${INSTALL_DIR_RESOURCE}/${RESOURCE_FOLDER}")
  else()
    OCCT_INSTALL_FILE_OR_DIR ("src/${RESOURCE}" "${INSTALL_DIR_RESOURCE}")
  endif()
endforeach()

# patch installed DrawDefault file if BUILD_SHARED_LIBRARY_NAME_POSTFIX is changed
if (NOT "${BUILD_SHARED_LIBRARY_NAME_POSTFIX}" STREQUAL "")
  OCCT_UPDATE_DRAW_DEFAULT_FILE()
endif()

# include patched toolkit projects or original ones
foreach (BUILD_TOOLKIT ${BUILD_TOOLKITS})
  OCCT_ADD_SUBDIRECTORY ("src/${BUILD_TOOLKIT}")
endforeach()

if (BUILD_DOC_Overview)
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

# include patched toolkit projects or original ones
if (BUILD_TOOL_TOOLKITS)
  foreach (BUILD_TOOL_TOOLKIT ${BUILD_TOOL_TOOLKITS})
    OCCT_ADD_SUBDIRECTORY ("tools/${BUILD_TOOL_TOOLKIT}")
  endforeach()
endif()

message (STATUS "Info: \(${CURRENT_TIME}\) OCCT toolkits processed")
# samples do not support patch usage
if (BUILD_MODULE_MfcSamples)
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

  message (STATUS "Info: \(${CURRENT_TIME}\) MFC Sample projects added")
endif()

if (BUILD_MODULE_UwpSample)
  add_subdirectory(samples/xaml)
endif()

if (BUILD_TOOL_TOOLKITS)
  # copy tinspector script to install script folder
  if (BUILD_PATCH AND EXISTS "${BUILD_PATCH}/adm/templates/inspector.${SCRIPT_EXT}")
    install (FILES "${BUILD_PATCH}/adm/templates/inspector.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR_SCRIPT}"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  else()
    install (FILES "${CMAKE_SOURCE_DIR}/adm/templates/inspector.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR_SCRIPT}"
            PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  endif()

  set (BUILD_SAMPLE_TOOLKITS TInspectorEXE)
  foreach (BUILD_SAMPLE_TOOL_TOOLKIT ${BUILD_SAMPLE_TOOLKITS})
    OCCT_ADD_SUBDIRECTORY ("samples/tools/${BUILD_SAMPLE_TOOL_TOOLKIT}")
  endforeach()

  # patch TInspectorEXE
  OCCT_CONFIGURE ("adm/templates/TInspectorEXE.vcxproj.user.in" "${CMAKE_BINARY_DIR}/samples/tools/TInspectorEXE/TInspectorEXE.vcxproj.user")
endif()

# Prepare variables for configuration of OpenCASCADE cmake config file
set (OCCT_MODULES_ENABLED)
set (OCCT_LIBRARIES)
set (SET_OpenCASCADE_MODULES_TOOLKITS "\n# List of available OpenCASCADE libraries for each module\n")
set (OCCT_COMPILE_DEFINITIONS)

foreach (OCCT_TOOLKIT ${BUILD_TOOLKITS})
  if (TARGET ${OCCT_TOOLKIT})

    # get all libraries
    get_target_property (${OCCT_TOOLKIT}_TYPE ${OCCT_TOOLKIT} "TYPE")
    if ("${${OCCT_TOOLKIT}_TYPE}" STREQUAL "STATIC_LIBRARY" OR "${${OCCT_TOOLKIT}_TYPE}" STREQUAL "SHARED_LIBRARY" )
      list (APPEND OCCT_LIBRARIES ${OCCT_TOOLKIT})

      # get all modules
      get_target_property (${OCCT_TOOLKIT}_MODULE ${OCCT_TOOLKIT} "MODULE")
      list (APPEND OCCT_MODULES_ENABLED ${${OCCT_TOOLKIT}_MODULE})
      list (APPEND OpenCASCADE_${${OCCT_TOOLKIT}_MODULE}_TOOLKITS ${OCCT_TOOLKIT})

      # get compile definitions of target directory
      get_directory_property (COMPILE_DEFINITIONS DIRECTORY "${${OCCT_TOOLKIT}_SOURCE_DIR}" "COMPILE_DEFINITIONS")
      list (APPEND OCCT_COMPILE_DEFINITIONS ${COMPILE_DEFINITIONS})
    endif()
  endif()
endforeach()
list (REMOVE_DUPLICATES OCCT_MODULES_ENABLED)
list (REMOVE_DUPLICATES OCCT_COMPILE_DEFINITIONS)

# export compile definitions and C/C++ flags for each configuration to OpenCASCADE config files
foreach (OCCT_CONFIGURATION ${CMAKE_CONFIGURATION_TYPES})
  set (SET_OpenCASCADE_COMPILE_DEFINITIONS)
  string (TOUPPER "${OCCT_CONFIGURATION}" OCCT_CONFIGURATION_UPPER)
  string (TOLOWER "${OCCT_CONFIGURATION}" OCCT_CONFIGURATION_LOWER)
  foreach(COMPILE_DEFINITION ${OCCT_COMPILE_DEFINITIONS})
    string(REPLACE "(" "\\(" COMPILE_DEFINITION "${COMPILE_DEFINITION}")
    string(REPLACE ")" "\\)" COMPILE_DEFINITION "${COMPILE_DEFINITION}")
    set (SET_OpenCASCADE_COMPILE_DEFINITIONS "${SET_OpenCASCADE_COMPILE_DEFINITIONS}set_property(DIRECTORY APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:${OCCT_CONFIGURATION}>:${COMPILE_DEFINITION}>)\n")
  endforeach()
  set (SET_OpenCASCADE_CMAKE_C_FLAGS "${CMAKE_C_FLAGS_${OCCT_CONFIGURATION_UPPER}}")
  set (SET_OpenCASCADE_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_${OCCT_CONFIGURATION_UPPER}}")
  configure_file("${CMAKE_SOURCE_DIR}/adm/templates/OpenCASCADECompileDefinitionsAndFlags.cmake.in" "OpenCASCADECompileDefinitionsAndFlags-${OCCT_CONFIGURATION_LOWER}.cmake" @ONLY)
endforeach()
# install OpenCASCADE config file with compile definitions and C/C++ flags ONLY for current configuration
install (CODE "string (TOLOWER \"\${CMAKE_INSTALL_CONFIG_NAME}\" CMAKE_INSTALL_CONFIG_NAME_LOWER)")
install (CODE "configure_file(\"${CMAKE_BINARY_DIR}/OpenCASCADECompileDefinitionsAndFlags-\${CMAKE_INSTALL_CONFIG_NAME_LOWER}.cmake\" \"${INSTALL_DIR}/${INSTALL_DIR_CMAKE}/OpenCASCADECompileDefinitionsAndFlags-\${CMAKE_INSTALL_CONFIG_NAME_LOWER}.cmake\" COPYONLY)")

foreach (OCCT_MODULE ${OCCT_MODULES})
  if (BUILD_MODULE_${OCCT_MODULE})
    set (SET_OpenCASCADE_MODULES_TOOLKITS "${SET_OpenCASCADE_MODULES_TOOLKITS}set (OpenCASCADE_${OCCT_MODULE}_LIBRARIES ${OpenCASCADE_${OCCT_MODULE}_TOOLKITS})\n")
  endif()

  # sort enabled modules for correct work of OpenCASCADE config file
  list(FIND OCCT_MODULES_ENABLED "${OCCT_MODULE}" OCCT_MODULE_INDEX)
  if (NOT ${OCCT_MODULE_INDEX} EQUAL -1)
    list (APPEND OCCT_MODULES_ENABLED_SORTED ${OCCT_MODULE})
  endif()
endforeach()
set (OCCT_MODULES_ENABLED ${OCCT_MODULES_ENABLED_SORTED})

# Add all targets to the build-tree export set
export (TARGETS ${OCCT_LIBRARIES} FILE "${CMAKE_BINARY_DIR}/OpenCASCADETargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE OpenCASCADE)

if (CMAKE_BUILD_TYPE)
  set (SET_OpenCASCADE_BUILD_TYPE "set (OpenCASCADE_BUILD_TYPE        \"${CMAKE_BUILD_TYPE}\")")
endif()

if (BUILD_SHARED_LIBS)
  set (SET_OpenCASCADE_LINKER_FLAGS "set (OpenCASCADE_LINKER_FLAGS   \"${CMAKE_SHARED_LINKER_FLAGS}\")")
else()
  set (SET_OpenCASCADE_LINKER_FLAGS "set (OpenCASCADE_LINKER_FLAGS   \"${CMAKE_STATIC_LINKER_FLAGS}\")")
endif()

if (WIN32)
  set (SET_OpenCASCADE_WITH_D3D "set (OpenCASCADE_WITH_D3D       ${USE_D3D})")
endif()
if (APPLE)
  set (SET_OpenCASCADE_WITH_GLX "set (OpenCASCADE_WITH_GLX       ${USE_GLX})")
endif()

if (NOT SINGLE_GENERATOR)
  OCCT_INSERT_CODE_FOR_TARGET()
endif()

# Configure and install cmake config file
configure_file("${CMAKE_SOURCE_DIR}/adm/templates/OpenCASCADEConfig.cmake.in" "OpenCASCADEConfig.cmake" @ONLY)
install(FILES "${CMAKE_BINARY_DIR}/OpenCASCADEConfig.cmake" DESTINATION  "${INSTALL_DIR_CMAKE}")

# Configure cmake version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file( ${CMAKE_BINARY_DIR}/OpenCASCADEConfigVersion.cmake
                                  VERSION ${OCC_VERSION_MAJOR}.${OCC_VERSION_MINOR}.${OCC_VERSION_MAINTENANCE}
                                  COMPATIBILITY ExactVersion )

# Install cmake version file
install (FILES "${CMAKE_BINARY_DIR}/OpenCASCADEConfigVersion.cmake" DESTINATION "${INSTALL_DIR_CMAKE}")

# Install the export set for use with the install-tree for each configuration
foreach (OCCT_MODULE ${OCCT_MODULES_ENABLED})
  install(EXPORT OpenCASCADE${OCCT_MODULE}Targets DESTINATION "${INSTALL_DIR_CMAKE}")
endforeach()

# Update generated OpenCASCADETargets-*.cmake files 
# to have correct paths to libraries depending on the configuration
OCCT_UPDATE_TARGET_FILE ()

message (STATUS "Info: \(${CURRENT_TIME}\) OCCT configuration files prepared")
