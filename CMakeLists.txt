cmake_minimum_required (VERSION 2.8.12 FATAL_ERROR)

set (CMAKE_SUPPRESS_REGENERATION TRUE)

# set build configurations list
if (NOT BUILD_CONFIGURATION)
  set (BUILD_CONFIGURATION "Release" CACHE STRING "Build type of OCCT" FORCE)
  SET_PROPERTY(CACHE BUILD_CONFIGURATION PROPERTY STRINGS Release Debug RelWithDebInfo)
endif()

set (CMAKE_CONFIGURATION_TYPES ${BUILD_CONFIGURATION} CACHE INTERNAL "" FORCE)

# set type of OCCT libraries
if (NOT BUILD_LIBRARY_TYPE)
  set (BUILD_LIBRARY_TYPE "Shared" CACHE STRING "The type of OCCT libraries" FORCE)
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

# Generate documentation
# Overview and User Guides
set (DOC_GENERATE_OVERVIEW OFF CACHE BOOL "Generate Overview and User Guides")
if (NOT DOC_OVERVIEW_OUTPUT_FORMAT)
  set (DOC_OVERVIEW_OUTPUT_FORMAT "html" CACHE STRING "Output format of Overview and User Guides" FORCE)
  SET_PROPERTY (CACHE DOC_OVERVIEW_OUTPUT_FORMAT PROPERTY STRINGS html chm pdf)
endif()
# class Reference Manual
set (DOC_GENERATE_REFMAN OFF CACHE BOOL "Generate class Reference Manual")

# Tests
set (TESTS_RUN OFF CACHE BOOL "Run tests or not")
set (TESTS_INSTALL OFF CACHE BOOL "Copy tests to folder install")
set (TESTS_SHAPES_DIR "" CACHE PATH "Directory that will contain shapes for tests" )

# Find Bison and Flex executables to rebuild *.yacc and *.lex files if it is necessary (BUILD_BISON_FLEX_FILES is ON)
set (BUILD_BISON_FLEX_FILES OFF CACHE BOOL "Build *.yacc/*.lex files")
if (${BUILD_BISON_FLEX_FILES})
  foreach (aTool BISON FLEX)
    list (APPEND CMAKE_PROGRAM_PATH ${BUILD_BISON_FLEX_DIR})
    find_package (${aTool})
    if (NOT ${${aTool}_FOUND})
      message ("Warning : set BUILD_BISON_FLEX_DIR directory")
      set (BUILD_BISON_FLEX_DIR "" CACHE PATH "Filepath to BISON and FLEX executables")
      set (BUILD_BISON_FLEX_FILES OFF)
    endif()
  endforeach()
endif()

# copy samples to install directory
set (INSTALL_SAMPLES OFF CACHE BOOL "Copy OCCT samples to install directory")

set (CMAKE_BUILD_TYPE ${BUILD_CONFIGURATION} CACHE INTERNAL "Build type of OCCT" FORCE )

# install dir of the built project
set (INSTALL_DIR "" CACHE PATH "Directory that will contain install files of OCCT" )
set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )

set (BUILD_PATCH_DIR "" CACHE PATH "directory with occt patch")

# the list of being built toolkits
set (BUILD_TOOLKITS "" CACHE STRING "Toolkits are also included in OCCT")
separate_arguments (BUILD_TOOLKITS)

if (MSVC)
  set (BUILD_MFC_SAMPLES OFF CACHE BOOL "OCCT samples building")
endif()

# whether use optional 3rdparty or not
if (APPLE)
  set (USE_GLX OFF CACHE BOOL "Are X11 OpenGL used on OSX or not")
endif()

set (USE_FREEIMAGE OFF CACHE BOOL "Is freeimage used or not")
set (USE_VTK OFF CACHE BOOL "Is VTK used or not")

if (NOT DEFINED ANDROID)
  set (USE_GL2PS OFF CACHE BOOL "Is gl2ps used or not")
  set (USE_TBB OFF CACHE BOOL "Is tbb used or not")
  #set (USE_OPENCL OFF CACHE BOOL "Is OpenCL used or not")
endif()

# macro: include patched file if it exists
macro (OCCT_INCLUDE_CMAKE_FILE BEING_INCLUDED_FILE)
  if (NOT "${BUILD_PATCH_DIR}" STREQUAL "" AND EXISTS "${BUILD_PATCH_DIR}/${BEING_INCLUDED_FILE}.cmake")
    include (${BUILD_PATCH_DIR}/${BEING_INCLUDED_FILE}.cmake)
  else()
    include (${BEING_INCLUDED_FILE}.cmake)
  endif()
endmacro()

# include occt macros
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_macros")

# BUILD_POSTFIX variable is used by all toolkit cmakelists.txt projects
OCCT_MAKE_BUILD_POSTFIX()

# include the patched or original list of modules
# list <MODULENAME>_TOOLKITS is created foreach module and contains its toolkits
# list <OCCT_MODULES> will contain all modules
OCCT_MODULES_AND_TOOLKITS (OCCT_MODULES)

foreach (MODULE ${OCCT_MODULES})
  set (BUILD_${MODULE} ON CACHE BOOL "include ${MODULE}")
endforeach()

if (NOT USE_VTK)
  list (REMOVE_ITEM Visualization_TOOLKITS TKIVtk)
  list (REMOVE_ITEM Draw_TOOLKITS TKIVtkDraw)
endif()

# accumulate used toolkits (first level) in USED_TOOLKITS variable
list (APPEND USED_TOOLKITS ${BUILD_TOOLKITS})

foreach (MODULE ${OCCT_MODULES})
  if (BUILD_${MODULE})
    list (APPEND USED_TOOLKITS ${${MODULE}_TOOLKITS})
  endif()
endforeach()

# DRAWEXE excluded when library build is static
if (NOT BUILD_SHARED_LIBS)
  list (REMOVE_ITEM USED_TOOLKITS DRAWEXE)
  message (STATUS "Info: DRAWEXE is not included due to ${BUILD_LIBRARY_TYPE} build library type")
endif()

# accumulate all used toolkits
list (REMOVE_DUPLICATES USED_TOOLKITS)
set (RAW_USED_TOOLKIT)
foreach(USED_TOOLKIT ${USED_TOOLKITS})
  OCCT_TOOLKIT_FULL_DEP (${USED_TOOLKIT} TOOLKIT_FULL_DEPS)
  list (APPEND RAW_USED_TOOLKIT ${USED_TOOLKIT} ${TOOLKIT_FULL_DEPS})
endforeach()

list (REMOVE_DUPLICATES RAW_USED_TOOLKIT)
set (USED_TOOLKITS ${RAW_USED_TOOLKIT})

# include the patched or original list of definitions and flags
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_defs_flags")

OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/3rdparty_macro")

set (3RDPARTY_DIR_LABEL "The directory containing required 3rdparty products")
if (NOT DEFINED 3RDPARTY_DIR)
  set (3RDPARTY_DIR "" CACHE PATH ${3RDPARTY_DIR_LABEL})
endif()

# search for 3rdparty dir
if ("${3RDPARTY_DIR}" STREQUAL "")
  if (DEFINED ENV{3RDPARTY_DIR})
    set (3RDPARTY_DIR "$ENV{3RDPARTY_DIR}" CACHE PATH ${3RDPARTY_DIR_LABEL} FORCE)
  elseif (EXISTS "${CMAKE_SOURCE_DIR}/../")
    # in version 6.7.0 and above, occt parent directory contains 3rdparties
    get_filename_component (3RDPARTY_DIR "${CMAKE_SOURCE_DIR}/../" ABSOLUTE)
    set (3RDPARTY_DIR "${3RDPARTY_DIR}" CACHE PATH ${3RDPARTY_DIR_LABEL} FORCE)
  endif()
endif()

# search for CSF_TclLibs variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED(CSF_TclLibs USE_TCL)

if ("${USE_TCL}" STREQUAL ON)
  message (STATUS "Info: tcl is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tcl")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TCL")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TK")
endif()

# search for CSF_FREETYPE variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED(CSF_FREETYPE USE_FREETYPE)

if ("${USE_FREETYPE}" STREQUAL ON)
  message (STATUS "Info: freetype is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/freetype")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREETYPE")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build")
endif()

# VTK
if (USE_VTK)
  add_definitions (-DHAVE_VTK)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/vtk")
endif()

# GLX
if (USE_GLX)
  add_definitions (-DMACOSX_USE_GLX)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/glx")
endif()

# FREEIMAGE
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

# GL2PS
if (USE_GL2PS)
  add_definitions (-DHAVE_GL2PS)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/gl2ps")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_GL2PS")
  OCCT_CHECK_AND_UNSET ("INSTALL_GL2PS")
endif()

# OPENCL
#if (USE_OPENCL)
#  add_definitions (-DHAVE_OPENCL)
#  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/opencl")
#else()
#  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_OPENCL")
#  OCCT_CHECK_AND_UNSET ("3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_HEADER")
#  OCCT_CHECK_AND_UNSET ("3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_LIB")
#  OCCT_CHECK_AND_UNSET ("INSTALL_OPENCL")
#endif()

# TBB
if (USE_TBB)
  add_definitions (-DHAVE_TBB)
  OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/tbb")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TBB")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TBBMALLOC")
  OCCT_CHECK_AND_UNSET ("INSTALL_TBB")
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

# Get all used variables: OS_WITH_BIT, COMPILER, BUILD_POSTFIX
OCCT_MAKE_BUILD_POSTFIX()
OCCT_MAKE_OS_WITH_BITNESS()
OCCT_MAKE_COMPILER_SHORT_NAME()

# build directories
string (TOUPPER "${BUILD_CONFIGURATION}" TAIL_OF_OUTPUT_VARNAME)

set (OUTPUT_LIBRARY_DIR ${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/lib${BUILD_POSTFIX})
set (OUTPUT_BINARY_DIR ${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin${BUILD_POSTFIX})

set ("CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${TAIL_OF_OUTPUT_VARNAME}" "${OUTPUT_LIBRARY_DIR}")
set ("CMAKE_RUNTIME_OUTPUT_DIRECTORY_${TAIL_OF_OUTPUT_VARNAME}" "${OUTPUT_BINARY_DIR}")
set ("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TAIL_OF_OUTPUT_VARNAME}" "${OUTPUT_LIBRARY_DIR}")

if (WIN32)
  set ("CMAKE_LIBRARY_OUTPUT_DIRECTORY_${TAIL_OF_OUTPUT_VARNAME}" "${OUTPUT_BINARY_DIR}")
endif()

message (STATUS "\nInfo: Collecting all OCCT header files into ${CMAKE_BINARY_DIR}/inc ...")

# (!) patch is not taken into account COLLECT_AND_INSTALL_OCCT_HEADER_FILES
# collect all the headers to <binary dir>/inc folder
COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/adm/templates/header.in" "${CMAKE_BINARY_DIR}" "${USED_TOOLKITS}")
# apply patched header files
#if (NOT "${BUILD_PATCH_DIR}" STREQUAL "")
#  COLLECT_AND_INSTALL_OCCT_HEADER_FILES ("${BUILD_PATCH_DIR}" "${CMAKE_SOURCE_DIR}/adm/templates/header.in" "${CMAKE_BINARY_DIR}")
#endif()

if ("${INSTALL_DIR}" STREQUAL "")
  message (FATAL_ERROR "INSTALL_DIR variable is empty. It's required to define installation directory")
else()


  OCCT_INSTALL_FILE_OR_DIR ("data" "${INSTALL_DIR}")
  OCCT_INSTALL_FILE_OR_DIR ("samples/tcl" "${INSTALL_DIR}/samples")

  if (INSTALL_SAMPLES)
    OCCT_INSTALL_FILE_OR_DIR ("samples" "${INSTALL_DIR}")
  endif()

  if (TESTS_INSTALL)
    OCCT_INSTALL_FILE_OR_DIR ("tests" "${INSTALL_DIR}")
  endif()

  if (WIN32)
    set (SCRIPT_EXT bat)
  else()
    set (SCRIPT_EXT sh)
  endif()

  # Creation of "START_TESTS" project
  if (TESTS_RUN AND TESTS_INSTALL)
    #set(BUILD_TESTING ON)
    #enable_testing ()
    #add_test(NAME RUN_TESTS COMMAND ${INSTALL_DIR}/draw.${SCRIPT_EXT} -c testgrid -overwrite -outdir ${INSTALL_DIR}/TestResults)
    add_custom_target(START_TESTS ${INSTALL_DIR}/draw.${SCRIPT_EXT} -c testgrid -overwrite -outdir ${INSTALL_DIR}/TestResults DEPENDS INSTALL)
  endif()

  # Creation of "GenerateDocumentation" project
  if (DOC_GENERATE_OVERVIEW OR DOC_GENERATE_REFMAN)
    OCCT_INCLUDE_CMAKE_FILE("adm/cmake/occt_gendoc")
    if(DOC_GENERATE_OVERVIEW)
      gendoc(-overview -${DOC_OVERVIEW_OUTPUT_FORMAT})
    endif()
    if(DOC_GENERATE_REFMAN)
      gendoc(-refman -html)
    endif()
  endif()

  # DRAW.BAT or DRAW.SH
  if (NOT "${BUILD_PATCH_DIR}" STREQUAL "" AND EXISTS "${BUILD_PATCH_DIR}/adm/templates/draw.${SCRIPT_EXT}")
    install(FILES "${BUILD_PATCH_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  else()
    install(FILES "${CMAKE_SOURCE_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  endif()
  OCCT_COPY_FILE_OR_DIR ("adm/templates/draw.${SCRIPT_EXT}" "${CMAKE_BINARY_DIR}")

  set (SUB_CUSTOM "custom_${COMPILER}_${COMPILER_BITNESS}.${SCRIPT_EXT}")
  if ("${BUILD_CONFIGURATION}" STREQUAL "Debug")
    set (SUB_CUSTOM "custom_${COMPILER}_${COMPILER_BITNESS}_${BUILD_POSTFIX}.${SCRIPT_EXT}")
  endif()

  if (WIN32)
    set (ADDITIONAL_CUSTOM_CONTENT "\nif exist \"%~dp0${SUB_CUSTOM}\" (\n  call \"%~dp0${SUB_CUSTOM}\" %1 %2 %3 \n)")
  else()
    set (ADDITIONAL_CUSTOM_CONTENT "\nif [ -e \"\${aScriptPath}/${SUB_CUSTOM}\" ]; then\n  source \"\${aScriptPath}/${SUB_CUSTOM}\" \"\${COMPILER}\" \"\${WOKSTATION}\${ARCH}\" \"\${CASDEB}\" \nfi")
  endif()

   # change custom.bat/sh
  if (EXISTS "${INSTALL_DIR}/custom.${SCRIPT_EXT}")
    file (READ "${INSTALL_DIR}/custom.${SCRIPT_EXT}" CUSTOM_CONTENT)

    set (CUSTOM_CONTENT "${CUSTOM_CONTENT} ${ADDITIONAL_CUSTOM_CONTENT}")

    file (WRITE "${INSTALL_DIR}/custom.${SCRIPT_EXT}" "${CUSTOM_CONTENT}")
  else()
    OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.${SCRIPT_EXT}.main" "custom.${SCRIPT_EXT}" "${INSTALL_DIR}")
  endif()

  # write current custom.bat/sh
  OCCT_CONFIGURE_AND_INSTALL ("adm/templates/custom.${SCRIPT_EXT}.in" "${SUB_CUSTOM}" "${INSTALL_DIR}")

  if (BUILD_MFC_SAMPLES)
    OCCT_INSTALL_FILE_OR_DIR ("adm/templates/sample.bat" "${INSTALL_DIR}")
    OCCT_COPY_FILE_OR_DIR ("adm/templates/sample.bat" "${CMAKE_BINARY_DIR}")
  endif()

  OCCT_CONFIGURE ("adm/templates/env.build.${SCRIPT_EXT}.in" "env.${SCRIPT_EXT}")
  OCCT_CONFIGURE ("adm/templates/env.${SCRIPT_EXT}.in" "env.install.${SCRIPT_EXT}")
  install(FILES "${CMAKE_BINARY_DIR}/env.install.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" RENAME "env.${SCRIPT_EXT}")
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
foreach (USED_TOOLKIT ${USED_TOOLKITS})
  if (NOT "${BUILD_PATCH_DIR}" STREQUAL "" AND EXISTS "${BUILD_PATCH_DIR}/src/${USED_TOOLKIT}")
    add_subdirectory(${BUILD_PATCH_DIR}/src/${USED_TOOLKIT})
  elseif (EXISTS "${CMAKE_SOURCE_DIR}/src/${USED_TOOLKIT}")
    add_subdirectory (${CMAKE_SOURCE_DIR}/src/${USED_TOOLKIT})
  else()
    message (STATUS "${USED_TOOLKIT} is not included")
  endif()
endforeach()

# patch DRAWEXE
if (MSVC AND 3RDPARTY_DLL_DIRS)
  list (FIND USED_TOOLKITS DRAWEXE DRAWEXE_INDEX)
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

    configure_file (${CMAKE_SOURCE_DIR}/adm/templates/DRAWEXE.vcxproj.user.in ${CMAKE_BINARY_DIR}/src/DRAWEXE/DRAWEXE.vcxproj.user @ONLY)
  endif()
endif()

# samples do not support patch usage
if (BUILD_MFC_SAMPLES)
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
