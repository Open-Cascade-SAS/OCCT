cmake_minimum_required (VERSION 2.8.10 FATAL_ERROR)

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

set (CMAKE_BUILD_TYPE ${BUILD_CONFIGURATION} CACHE INTERNAL "Build type of OCCT" FORCE )

# install dir of the built project
set (INSTALL_DIR "" CACHE PATH "Directory that will contain install files of OCCT" )
set (CMAKE_INSTALL_PREFIX "${INSTALL_DIR}" CACHE INTERNAL "" FORCE )

# whether install test bundle or not
set (INSTALL_TESTS OFF CACHE BOOL "Is tests copy to install directory")

set (BUILD_PATCH_DIR "" CACHE PATH "directory with OCCT patch")

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
  set (USE_OPENCL OFF CACHE BOOL "Is OpenCL used or not")
endif()

# macro: include patched file if it exists
macro (OCCT_INCLUDE_CMAKE_FILE BEING_INCLUDED_FILE)
  if (NOT "${BUILD_PATCH_DIR}" STREQUAL "" AND EXISTS "${BUILD_PATCH_DIR}/${BEING_INCLUDED_FILE}.cmake")
    include(${BUILD_PATCH_DIR}/${BEING_INCLUDED_FILE}.cmake)
  else()
    include(${BEING_INCLUDED_FILE}.cmake)
  endif()
endmacro()

# include occt macros
OCCT_INCLUDE_CMAKE_FILE ("adm/templates/occt_macros")

# BUILD_POSTFIX variable is used by all toolkit cmakelists.txt projects
OCCT_MAKE_BUILD_POSTFIX()

# include the patched or original list of modules
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_modules")

# include the list of being used toolkits. USED_TOOLKITS variable
OCCT_INCLUDE_CMAKE_FILE ("adm/cmake/occt_toolkits")

# include the patched or original list of definitions and flags
OCCT_INCLUDE_CMAKE_FILE ("adm/templates/occt_defs_flags")

OCCT_INCLUDE_CMAKE_FILE ("adm/templates/3rdparty_macro")

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
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/tcl")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TCL")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_TK")
endif()

# search for CSF_FREETYPE variable in EXTERNLIB of each being used toolkit
OCCT_IS_PRODUCT_REQUIRED(CSF_FREETYPE USE_FREETYPE)

if ("${USE_FREETYPE}" STREQUAL ON)
  message (STATUS "Info: freetype is used by OCCT")
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/freetype")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREETYPE")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FREETYPE_INCLUDE_DIR_freetype2")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_FREETYPE_INCLUDE_DIR_ft2build")
endif()

# VTK
if (USE_VTK)
  add_definitions (-DHAVE_VTK)
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/vtk")
endif()

# GLX
if (USE_GLX)
  add_definitions (-DMACOSX_USE_GLX)
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/glx")
endif()

# FREEIMAGE
if (USE_FREEIMAGE)
  add_definitions (-DHAVE_FREEIMAGE)
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/freeimage")
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/freeimageplus")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGE")
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_FREEIMAGEPLUS")
  OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGE")
  OCCT_CHECK_AND_UNSET ("INSTALL_FREEIMAGEPLUS")
endif()

# GL2PS
if (USE_GL2PS)
  add_definitions (-DHAVE_GL2PS)
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/gl2ps")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_GL2PS")
  OCCT_CHECK_AND_UNSET ("INSTALL_GL2PS")
endif()

# OPENCL
if (USE_OPENCL)
  add_definitions (-DHAVE_OPENCL)
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/opencl")
else()
  OCCT_CHECK_AND_UNSET_GROUP ("3RDPARTY_OPENCL")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_HEADER")
  OCCT_CHECK_AND_UNSET ("3RDPARTY_OPENCL_ADDITIONAL_PATH_FOR_LIB")
  OCCT_CHECK_AND_UNSET ("INSTALL_OPENCL")
endif()

# TBB
if (USE_TBB)
  add_definitions (-DHAVE_TBB)
  OCCT_INCLUDE_CMAKE_FILE ("adm/templates/tbb")
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

if (3RDPARTY_LIBRARY_DIRS)
  list (REMOVE_DUPLICATES 3RDPARTY_LIBRARY_DIRS)
  string (REGEX REPLACE ";" "\n\t" 3RDPARTY_LIBRARY_DIRS_WITH_ENDS "${3RDPARTY_LIBRARY_DIRS}")
  message (STATUS "The directories containing 3rdparty libraries: ${3RDPARTY_LIBRARY_DIRS_WITH_ENDS}")
  link_directories (${3RDPARTY_LIBRARY_DIRS})
endif()

OCCT_MAKE_BUILD_POSTFIX()

# build directories
set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib${BUILD_POSTFIX})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/lib${BUILD_POSTFIX})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/out/bin${BUILD_POSTFIX})

if ("${INSTALL_DIR}" STREQUAL "")
  message (FATAL_ERROR "INSTALL_DIR variable is empty. It's required to define installation directory")
else()
  # inc,data,tests DIRECTORY
  install (DIRECTORY "${CMAKE_SOURCE_DIR}/inc"          DESTINATION  "${INSTALL_DIR}")
  install (DIRECTORY "${CMAKE_SOURCE_DIR}/data"         DESTINATION  "${INSTALL_DIR}")
  install (DIRECTORY "${CMAKE_SOURCE_DIR}/samples/tcl"  DESTINATION  "${INSTALL_DIR}/samples")
  if (INSTALL_TESTS)
    install (DIRECTORY "${CMAKE_SOURCE_DIR}/tests" DESTINATION  "${INSTALL_DIR}" )
  endif()

  # install patch inc, data, tests folder
  if (NOT "${BUILD_PATCH_DIR}" STREQUAL "")
    if (EXISTS "${BUILD_PATCH_DIR}/inc")
      install (DIRECTORY "${BUILD_PATCH_DIR}/inc" DESTINATION  "${INSTALL_DIR}" )
    endif()

    if (EXISTS "${BUILD_PATCH_DIR}/data")
      install (DIRECTORY "${BUILD_PATCH_DIR}/data"  DESTINATION  "${INSTALL_DIR}" )
    endif()

    if (INSTALL_TESTS)
      if (EXISTS "${BUILD_PATCH_DIR}/tests")
        install (DIRECTORY "${BUILD_PATCH_DIR}/tests" DESTINATION  "${INSTALL_DIR}" )
      endif()
    endif()
  endif()

  if (WIN32)
    set (SCRIPT_EXT bat)
  else()
    set (SCRIPT_EXT sh)
  endif()

  # DRAW.BAT or DRAW.SH
  IF(NOT "${BUILD_PATCH_DIR}" STREQUAL "" AND EXISTS "${BUILD_PATCH_DIR}/adm/templates/draw.${SCRIPT_EXT}")
    install(FILES "${BUILD_PATCH_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  ELSE()
    install(FILES "${CMAKE_SOURCE_DIR}/adm/templates/draw.${SCRIPT_EXT}" DESTINATION  "${INSTALL_DIR}" PERMISSIONS
            OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_WRITE WORLD_EXECUTE)
  ENDIF()

  # set compiler short name
  OCCT_MAKE_COMPILER_SHORT_NAME()

  OCCT_MAKE_COMPILER_BITNESS()

  SET(SUB_CUSTOM "custom_${COMPILER}_${COMPILER_BITNESS}_${BUILD_POSTFIX}.${SCRIPT_EXT}")

  if (WIN32)
    SET (ADDITIONAL_CUSTOM_CONTENT "\nif exist \"%~dp0${SUB_CUSTOM}\" (\n  call \"%~dp0${SUB_CUSTOM}\" %1 %2 %3 \n)")
  else()
    SET (ADDITIONAL_CUSTOM_CONTENT "\nif [ -e \"\${aScriptPath}/${SUB_CUSTOM}\" ]; then\n  source \"\${aScriptPath}/${SUB_CUSTOM}\" \"\${COMPILER}\" \"\${WOKSTATION}\${ARCH}\" \"\${CASDEB}\" \nfi")
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
  endif()

  OCCT_CONFIGURE_AND_INSTALL ("adm/templates/env.${SCRIPT_EXT}.in" "env.${SCRIPT_EXT}" "${INSTALL_DIR}")
endif()

# RESOURCES
OCCT_INSTALL_FILE_OR_DIR ("src/DrawResources"          "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/StdResource"            "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/SHMessage"              "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/Textures"               "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/Shaders"                "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/XSMessage"              "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/XSTEPResource"          "${INSTALL_DIR}/src")
OCCT_INSTALL_FILE_OR_DIR ("src/XmlOcafResource"        "${INSTALL_DIR}/src")

OCCT_INSTALL_FILE_OR_DIR ("src/UnitsAPI/Lexi_Expr.dat" "${INSTALL_DIR}/src/UnitsAPI")
OCCT_INSTALL_FILE_OR_DIR ("src/UnitsAPI/Units.dat"     "${INSTALL_DIR}/src/UnitsAPI")
OCCT_INSTALL_FILE_OR_DIR ("src/TObj/TObj.msg"          "${INSTALL_DIR}/src/TObj")


#Toolkits uses variables: INSTALL_DIR, OS_WITH_BIT, COMPILER, BUILD_POSTFIX
OCCT_MAKE_OS_WITH_BITNESS()

# consider for patch existence
set (IS_PATCH_CURRENT "NO")
set (TK_ROOT_DIR ${CMAKE_SOURCE_DIR})
if (NOT "${BUILD_PATCH_DIR}" STREQUAL "")
  set (IS_PATCH_CURRENT "YES")
  set (TK_ROOT_DIR ${BUILD_PATCH_DIR})
endif()

# include patched toolkit projects or original ones
set (UNSUBDIRS "")
if (NOT "${BUILD_PATCH_DIR}" STREQUAL "" AND EXISTS "${BUILD_PATCH_DIR}/adm/cmake/occt_inc_toolkits.cmake")
  set (TK_ROOT_DIR ${BUILD_PATCH_DIR})
  include (${BUILD_PATCH_DIR}/adm/cmake/occt_inc_toolkits.cmake)
else()
  set (IS_PATCH_CURRENT "NO")
  set (TK_ROOT_DIR ${CMAKE_SOURCE_DIR})
  include (adm/cmake/occt_inc_toolkits.cmake)
endif()

# include some required original occt_inc_toolkits
if (NOT "${UNSUBDIRS}" STREQUAL "")
  set (IS_PATCH_CURRENT "NO")
  # add required subdirs
  foreach (UNSUBDIR ${UNSUBDIRS})
    add_subdirectory (${CMAKE_SOURCE_DIR}/${UNSUBDIR})
  endforeach()
endif()

# samples do not support patch usage
IF (BUILD_MFC_SAMPLES)
  SET (OCCT_ROOT ${CMAKE_SOURCE_DIR})

  SET (MFC_STANDARD_SAMPLES_DIR ${OCCT_ROOT}/samples/mfc/standard)
  SET (COMMON_WINMAIN_FILE      ${MFC_STANDARD_SAMPLES_DIR}/Common/Winmain.cpp)

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
ENDIF()
