cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

set (CMAKE_SUPPRESS_REGENERATION TRUE)

set (CMAKE_CONFIGURATION_TYPES Release Debug RelWithDebInfo CACHE INTERNAL "" FORCE)

# macro: include patched file if it exists
macro (OCCT_INCLUDE_CMAKE_FILE BEING_INCLUDED_FILE)
  if (APPLY_OCCT_PATCH_DIR AND EXISTS "${APPLY_OCCT_PATCH_DIR}/${BEING_INCLUDED_FILE}.cmake")
    include (${APPLY_OCCT_PATCH_DIR}/${BEING_INCLUDED_FILE}.cmake)
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

# Solution folder property
set_property (GLOBAL PROPERTY USE_FOLDERS ON)

set (INSTALL_OCCT_TEST_CASES OFF CACHE BOOL "${INSTALL_OCCT_TEST_CASES_DESCR}")
# a single-configuration generator like the Makefile generator defines CMAKE_BUILD_TYPE variable
# check this variable and set if it's required
if (DEFINED CMAKE_BUILD_TYPE AND NOT CMAKE_BUILD_TYPE) # single-configuration generator.
  set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

# enable extended messages of many OCCT algorithms
set (OCCT_ALGO_EXTENDED_OUTPUT OFF CACHE BOOL "${OCCT_ALGO_EXTENDED_OUTPUT_DESCR}")
if (OCCT_ALGO_EXTENDED_OUTPUT)
  add_definitions (-DOCCT_DEBUG)
endif()

# copy samples to install directory
set (INSTALL_OCCT_SAMPLES OFF CACHE BOOL "${INSTALL_OCCT_SAMPLES_DESCR}")

# install dir of the built project
set (INSTALL_DIR "" CACHE PATH "${INSTALL_DIR_DESCR}" )
set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )

set (APPLY_OCCT_PATCH_DIR "" CACHE PATH "${APPLY_OCCT_PATCH_DIR_DESCR}")

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

set (USE_VTK OFF CACHE BOOL "${USE_VTK_DESCR}")

# include occt macros
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# include the patched or original list of modules
# list <MODULENAME>_TOOLKITS is created foreach module and contains its toolkits
# list <OCCT_MODULES> will contain all modules
OCCT_MODULES_AND_TOOLKITS (OCCT_MODULES)

foreach (OCCT_MODULE ${OCCT_MODULES})
  BUILD_MODULE (${OCCT_MODULE})
endforeach()

# Overview
set (BUILD_DOC_OcctOverview OFF CACHE BOOL "${BUILD_DOC_OcctOverview_DESCR}")

if (NOT USE_VTK)
  list (REMOVE_ITEM Visualization_TOOLKITS TKIVtk)
  list (REMOVE_ITEM Draw_TOOLKITS TKIVtkDraw)
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
foreach(BUILD_TOOLKIT ${BUILD_TOOLKITS})
  OCCT_TOOLKIT_FULL_DEP (${BUILD_TOOLKIT} TOOLKIT_FULL_DEPS)
  list (APPEND RAW_BUILD_TOOLKITS ${BUILD_TOOLKIT} ${TOOLKIT_FULL_DEPS})
endforeach()

list (REMOVE_DUPLICATES RAW_BUILD_TOOLKITS)
set (BUILD_TOOLKITS ${RAW_BUILD_TOOLKITS})

# include the patched or original list of definitions and flags
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_defs_flags")

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/3rdparty_macro")

# Rebuild *.yacc and *.lex files that are contained by TKMath toolkit
list (FIND BUILD_TOOLKITS TKMath   CAN_REBUILD_PDC_FOR_TKMATH)
list (FIND BUILD_TOOLKITS StepFile CAN_REBUILD_PDC_FOR_STEPFILE)

if (NOT "${CAN_REBUILD_PDC_FOR_TKMATH}" STREQUAL "-1" OR NOT "${CAN_REBUILD_PDC_FOR_STEPFILE}" STREQUAL "-1")
  set (REBUILD_PLATFORM_DEPENDENT_CODE OFF CACHE BOOL "${REBUILD_PLATFORM_DEPENDENT_CODE_DESCR}")
else()
  OCCT_CHECK_AND_UNSET (REBUILD_PLATFORM_DEPENDENT_CODE)
endif()

if (NOT DEFINED 3RDPARTY_DIR)
  set (3RDPARTY_DIR "" CACHE PATH ${3RDPARTY_DIR_DESCR})
endif()

# search for 3rdparty dir
if ("${3RDPARTY_DIR}" STREQUAL "")
  if (DEFINED ENV{3RDPARTY_DIR})
    set (3RDPARTY_DIR "$ENV{3RDPARTY_DIR}" CACHE PATH ${3RDPARTY_DIR_DESCR} FORCE)
  elseif (EXISTS "${CMAKE_SOURCE_DIR}/../")
    # in version 6.7.0 and above, occt parent directory contains 3rdparties
    get_filename_component (3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/../" ABSOLUTE)
    set (3RDPARTY_DIR "${3RDPARTY_DIR}" CACHE PATH ${3RDPARTY_DIR_DESCR} FORCE)
  endif()
endif()

# search for CSF_TclLibs variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_TclLibs USE_TCL)

if ("${USE_TCL}" STREQUAL ON)
  message (STATUS "Info: TCL is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tcl")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TCL")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TK")

  OCCT_CHECK_AND_UNSET ("INSTALL_TCL")
endif()

# search for CSF_FREETYPE variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED (CSF_FREETYPE USE_FREETYPE)

if ("${USE_FREETYPE}" STREQUAL ON)
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
  OCCT_CHECK_AND_UNSET ("INSTALL_VTK")
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
    install (DIRECTORY "${CMAKE_BINARY_DIR}/doc/overview" DESTINATION  "${INSTALL_DIR}/doc")

    OCCT_INSTALL_FILE_OR_DIR ("dox/resources/overview.html" "${INSTALL_DIR}")
  endif()

  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/doxygen")
else()
  OCCT_CHECK_AND_UNSET ("INSTALL_DOC_OcctOverview")

  OCCT_CHECK_AND_UNSET ("3RDPARTY_DOXYGEN_EXECUTABLE")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_DOXYGEN_DOT_EXECUTABLE")
endif()

# bison 
if (REBUILD_PLATFORM_DEPENDENT_CODE)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/bison")
else()
  OCCT_CHECK_AND_UNSET ("3RDPARTY_BISON_EXECUTABLE")
endif()

# flex 
if (REBUILD_PLATFORM_DEPENDENT_CODE)
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
  message (STATUS "The directories containing 3rdparty headers: ${3RDPARTY_INCLUDE_DIRS_WITH_ENDS}")
  include_directories (${3RDPARTY_INCLUDE_DIRS})
endif()

# include <cmake binary folder>/inc
include_directories (${CMAKE_BINARY_DIR}/inc)

if (3RDPARTY_LIBRARY_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_LIBRARY_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_LIBRARY_DIRS_WITH_ENDS "${3RDPARTY_LIBRARY_DIRS}")
  message (STATUS "The directories containing 3rdparty libraries: ${3RDPARTY_LIBRARY_DIRS_WITH_ENDS}")
  link_directories (${3RDPARTY_LIBRARY_DIRS})
endif()

# Get all used variables: OS_WITH_BIT, COMPILER
OCCT_MAKE_OS_WITH_BITNESS()
OCCT_MAKE_COMPILER_SHORT_NAME()

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

if ("${INSTALL_DIR}" STREQUAL "")
  get_filename_component (CMAKE_BINARY_DIR_PARENT ${CMAKE_BINARY_DIR} DIRECTORY)

  set (INSTALL_DIR "${CMAKE_BINARY_DIR_PARENT}/install" CACHE PATH "${INSTALL_DIR_DESCR}" FORCE)
  set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )

  message (STATUS "Info: INSTALL_DIR has been set as ${INSTALL_DIR}")
endif()

message (STATUS "\nInfo: Collecting all OCCT header files into ${CMAKE_BINARY_DIR}/inc ...")

# collect all the headers to <binary dir>/inc folder
COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_BINARY_DIR}" "${BUILD_TOOLKITS}")

OCCT_INSTALL_FILE_OR_DIR ("data" "${INSTALL_DIR}")
OCCT_INSTALL_FILE_OR_DIR ("samples/tcl" "${INSTALL_DIR}/samples")

if (INSTALL_OCCT_SAMPLES)
  OCCT_INSTALL_FILE_OR_DIR ("samples" "${INSTALL_DIR}")
endif()

if (INSTALL_OCCT_TEST_CASES)
  OCCT_INSTALL_FILE_OR_DIR ("tests" "${INSTALL_DIR}")
endif()

if (WIN32)
  set (SCRIPT_EXT bat)
else()
  set (SCRIPT_EXT sh)
endif()

# DRAW.BAT or DRAW.SH
if (APPLY_OCCT_PATCH_DIR AND EXISTS "${APPLY_OCCT_PATCH_DIR}/adm/templates/draw.${SCRIPT_EXT}")
  install (FILES "${APPLY_OCCT_PATCH_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
else()
  install (FILES "${CMAKE_SOURCE_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" PERMISSIONS
          OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
endif()
OCCT_COPY_FILE_OR_DIR ("adm/templates/draw.${SCRIPT_EXT}" "${CMAKE_BINARY_DIR}")

set (SUB_CUSTOM_NAME "custom_${COMPILER}_${COMPILER_BITNESS}.${SCRIPT_EXT}")

if (WIN32)
  set (ADDITIONAL_CUSTOM_CONTENT "\nif exist \"%~dp0${SUB_CUSTOM_NAME}\" (\n  call \"%~dp0${SUB_CUSTOM_NAME}\" %VCVER% %ARCH% %CASDEB% \n)")
else()
  set (ADDITIONAL_CUSTOM_CONTENT "\nif [ -e \"\${aScriptPath}/${SUB_CUSTOM_NAME}\" ]; then\n  source \"\${aScriptPath}/${SUB_CUSTOM_NAME}\" \"\${COMPILER}\" \"\${WOKSTATION}\${ARCH}\" \"\${CASDEB}\" \nfi")
endif()

 # change custom.bat/sh
if (EXISTS "${INSTALL_DIR}/custom.${SCRIPT_EXT}")
  file (READ "${INSTALL_DIR}/custom.${SCRIPT_EXT}" CUSTOM_CONTENT)

  set (CUSTOM_CONTENT "${CUSTOM_CONTENT} ${ADDITIONAL_CUSTOM_CONTENT}")

  file (WRITE "${INSTALL_DIR}/custom.${SCRIPT_EXT}" "${CUSTOM_CONTENT}")
else()
  OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.${SCRIPT_EXT}.main" "custom.${SCRIPT_EXT}" "custom.${SCRIPT_EXT}" "${INSTALL_DIR}")
endif()

# write current custom.bat/sh (for install directory)
set (SUB_CUSTOM_BUILD_NAME "custom_${COMPILER}_${COMPILER_BITNESS}.install.${SCRIPT_EXT}")
OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.install.${SCRIPT_EXT}.in" "${SUB_CUSTOM_BUILD_NAME}" "${SUB_CUSTOM_NAME}" "${INSTALL_DIR}")

# write current custom.bat/sh (for build directory)
OCCT_CONFIGURE ("adm/templates/custom.build.${SCRIPT_EXT}.in" "${SUB_CUSTOM_NAME}")

if (BUILD_MODULE_OcctMfcSamples)
  OCCT_INSTALL_FILE_OR_DIR ("adm/templates/sample.bat" "${INSTALL_DIR}")
  OCCT_COPY_FILE_OR_DIR    ("adm/templates/sample.bat" "${CMAKE_BINARY_DIR}")
endif()

# env script for draw in building environment
OCCT_CONFIGURE ("adm/templates/env.build.${SCRIPT_EXT}.in" "env.${SCRIPT_EXT}")

# end script for draw in standalone environment
# release version
set (BUILD_LETTER "")
OCCT_CONFIGURE ("adm/templates/env.install.${SCRIPT_EXT}.in" "env.install.release.${SCRIPT_EXT}")
install (FILES "${CMAKE_BINARY_DIR}/env.install.release.${SCRIPT_EXT}" 
         CONFIGURATIONS Release
         DESTINATION  "${INSTALL_DIR}"
         RENAME "env.${SCRIPT_EXT}")

# debug version
set (BUILD_LETTER "d")
OCCT_CONFIGURE ("adm/templates/env.install.${SCRIPT_EXT}.in" "env.install.debug.${SCRIPT_EXT}")
install (FILES "${CMAKE_BINARY_DIR}/env.install.debug.${SCRIPT_EXT}" 
         CONFIGURATIONS Debug
         DESTINATION  "${INSTALL_DIR}"
         RENAME "env.${SCRIPT_EXT}")

# release with debug info version
set (BUILD_LETTER "i")
OCCT_CONFIGURE ("adm/templates/env.install.${SCRIPT_EXT}.in" "env.install.relwithdebinfo.${SCRIPT_EXT}")
install (FILES "${CMAKE_BINARY_DIR}/env.install.relwithdebinfo.${SCRIPT_EXT}" 
         CONFIGURATIONS RelWithDebInfo
         DESTINATION  "${INSTALL_DIR}"
         RENAME "env.${SCRIPT_EXT}")

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
    OCCT_INSTALL_FILE_OR_DIR ("src/${RESOURCE}" "${INSTALL_DIR}/src/${RESOURCE_FOLDER}")
  else()
    OCCT_INSTALL_FILE_OR_DIR ("src/${RESOURCE}" "${INSTALL_DIR}/src")
  endif()
endforeach()

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
