# This script finds OpenCASCADE Technology libraries.
# The script requires:
#  OpenCASCADE_DIR - root OCCT folder or folder with CMake configuration files
#
# Script will define the following variables on success:
#  OpenCASCADE_FOUND       - package is successfully found
#  OpenCASCADE_INCLUDE_DIR - directory with headers
#  OpenCASCADE_LIBRARY_DIR - directory with libraries for linker
#  OpenCASCADE_BINARY_DIR  - directory with DLLs
include(FindPackageHandleStandardArgs)

# MY_PLATFORM variable
math (EXPR MY_BITNESS "32 + 32*(${CMAKE_SIZEOF_VOID_P}/8)")
if (WIN32)
  set (MY_PLATFORM "win${MY_BITNESS}")
elseif(APPLE)
  set (MY_PLATFORM "mac")
else()
  set (MY_PLATFORM "lin")
endif()

# MY_PLATFORM_AND_COMPILER variable
if (MSVC)
  if (MSVC90)
    set (MY_COMPILER vc9)
  elseif (MSVC10)
    set (MY_COMPILER vc10)
  elseif (MSVC11)
    set (MY_COMPILER vc11)
  elseif (MSVC12)
    set (MY_COMPILER vc12)
  elseif (MSVC14)
    set (MY_COMPILER vc14)
  else()
    set (MY_COMPILER vc15)
    message (WARNING "Unknown msvc version. $$MY_COMPILER is used")
  endif()
elseif (DEFINED CMAKE_COMPILER_IS_GNUCC)
  set (MY_COMPILER gcc)
elseif (DEFINED CMAKE_COMPILER_IS_GNUCXX)
  set (MY_COMPILER gcc)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set (MY_COMPILER clang)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
  set (MY_COMPILER icc)
else()
  set (MY_COMPILER ${CMAKE_GENERATOR})
  string (REGEX REPLACE " " "" COMPILER ${MY_COMPILER})
endif()
set (MY_PLATFORM_AND_COMPILER "${MY_PLATFORM}/${MY_COMPILER}")

set (OpenCASCADE_DIR "" CACHE PATH "Path to Open CASCADE libraries.")

# default paths
set (OpenCASCADE_INCLUDE_DIR "${OpenCASCADE_DIR}/inc")
set (OpenCASCADE_LIBRARY_DIR "${OpenCASCADE_DIR}/${MY_PLATFORM_AND_COMPILER}/lib")
set (OpenCASCADE_BINARY_DIR  "${OpenCASCADE_DIR}/${MY_PLATFORM_AND_COMPILER}/bin")

# complete list of OCCT Toolkits (copy-paste from adm/UDLIST, since installed OCCT does not include UDLIST)
set (OpenCASCADE_TKLIST "")
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKernel TKMath) # FoundationClasses
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKG2d TKG3d TKGeomBase TKBRep) # ModelingData
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKGeomAlgo TKTopAlgo TKPrim TKBO TKBool TKHLR TKFillet TKOffset TKFeat TKMesh TKXMesh TKShHealing) # ModelingAlgorithms
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKService TKV3d TKOpenGl TKMeshVS TKIVtk TKD3DHost) # Visualization
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKCDF TKLCAF TKCAF TKBinL TKXmlL TKBin TKXml TKStdL TKStd TKTObj TKBinTObj TKXmlTObj TKVCAF) # ApplicationFramework
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKXSBase TKSTEPBase TKSTEPAttr TKSTEP209 TKSTEP TKIGES TKXCAF TKXDEIGES TKXDESTEP TKSTL TKVRML TKXmlXCAF TKBinXCAF TKRWMesh) # DataExchange
set (OpenCASCADE_TKLIST ${OpenCASCADE_TKLIST} TKDraw TKViewerTest) # Draw

# validate location of OCCT libraries and headers
set (OpenCASCADE_INCLUDE_DIR_FOUND)
set (OpenCASCADE_LIBRARY_DIR_FOUND)
set (OpenCASCADE_LIBRARY_DEBUG_DIR_FOUND)
set (OpenCASCADE_IMPLIB_SUFFIX ${CMAKE_STATIC_LIBRARY_SUFFIX})
set (OpenCASCADE_SHAREDLIB_RELEASE_FOUND)
set (OpenCASCADE_SHAREDLIB_DEBUG_FOUND)
if (EXISTS "${OpenCASCADE_INCLUDE_DIR}/Standard.hxx")
  set (OpenCASCADE_INCLUDE_DIR_FOUND ON)
endif()

if (EXISTS "${OpenCASCADE_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set (OpenCASCADE_LIBRARY_DIR_FOUND ON)
elseif (NOT WIN32 AND EXISTS "${OpenCASCADE_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_SHARED_LIBRARY_SUFFIX}")
  set (OpenCASCADE_LIBRARY_DIR_FOUND ON)
  set (OpenCASCADE_IMPLIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
endif()

if (EXISTS "${OpenCASCADE_LIBRARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_STATIC_LIBRARY_SUFFIX}")
  set (OpenCASCADE_LIBRARY_DEBUG_DIR_FOUND ON)
elseif (NOT WIN32 AND EXISTS "${OpenCASCADE_LIBRARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_SHARED_LIBRARY_SUFFIX}")
  set (OpenCASCADE_LIBRARY_DEBUG_DIR_FOUND ON)
  set (OpenCASCADE_IMPLIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
elseif (OpenCASCADE_LIBRARY_DIR_FOUND)
  message (STATUS "Only release OpenCASCADE libraries have been found")
endif()

if (NOT OpenCASCADE_LIBRARY_DIR_FOUND AND OpenCASCADE_LIBRARY_DEBUG_DIR_FOUND)
  set (OpenCASCADE_LIBRARY_DIR_FOUND ON)
  message (WARNING "Only debug OpenCASCADE libraries have been found")
endif()

if (WIN32)
  if (EXISTS "${OpenCASCADE_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (OpenCASCADE_SHAREDLIB_RELEASE_FOUND ON)
  endif()
  if (EXISTS "${OpenCASCADE_BINARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (OpenCASCADE_SHAREDLIB_DEBUG_FOUND ON)
  endif()
else()
  if (EXISTS "${OpenCASCADE_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (OpenCASCADE_SHAREDLIB_RELEASE_FOUND ON)
  endif()
  if (EXISTS "${OpenCASCADE_LIBRARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}TKernel${CMAKE_SHARED_LIBRARY_SUFFIX}")
    set (OpenCASCADE_SHAREDLIB_DEBUG_FOUND ON)
  endif()
endif()

if (OpenCASCADE_INCLUDE_DIR_FOUND AND OpenCASCADE_LIBRARY_DIR_FOUND)
  set (OpenCASCADE_FOUND ON)
  set (OpenCASCADE_INSTALL_PREFIX ${OpenCASCADE_DIR})

  # Define OCCT toolkits so that CMake can put absolute paths to linker;
  # the library existance is not checked here, since modules can be disabled.
  foreach (aLibIter ${OpenCASCADE_TKLIST})
    add_library (${aLibIter} SHARED IMPORTED)

    set_property (TARGET ${aLibIter} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
    set_target_properties (${aLibIter} PROPERTIES IMPORTED_IMPLIB_RELEASE "${OpenCASCADE_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${aLibIter}${OpenCASCADE_IMPLIB_SUFFIX}")
    if (OpenCASCADE_SHAREDLIB_RELEASE_FOUND)
      if (WIN32)
        set_target_properties (${aLibIter} PROPERTIES IMPORTED_LOCATION_RELEASE "${OpenCASCADE_BINARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${aLibIter}${CMAKE_SHARED_LIBRARY_SUFFIX}")
      else()
        set_target_properties (${aLibIter} PROPERTIES IMPORTED_LOCATION_RELEASE "${OpenCASCADE_LIBRARY_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}${aLibIter}${CMAKE_SHARED_LIBRARY_SUFFIX}")
      endif()
    endif()

    if (OpenCASCADE_LIBRARY_DEBUG_DIR_FOUND)
      set_property (TARGET ${aLibIter} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
      set_target_properties (${aLibIter} PROPERTIES IMPORTED_IMPLIB_DEBUG "${OpenCASCADE_LIBRARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}${aLibIter}${OpenCASCADE_IMPLIB_SUFFIX}")
      if (OpenCASCADE_SHAREDLIB_DEBUG_FOUND)
        if (WIN32)
          set_target_properties (${aLibIter} PROPERTIES IMPORTED_LOCATION_DEBUG "${OpenCASCADE_BINARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}${aLibIter}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        else()
          set_target_properties (${aLibIter} PROPERTIES IMPORTED_LOCATION_DEBUG "${OpenCASCADE_LIBRARY_DIR}d/${CMAKE_SHARED_LIBRARY_PREFIX}${aLibIter}${CMAKE_SHARED_LIBRARY_SUFFIX}")
        endif()
      endif()
    endif()
  endforeach()
else()
  # fallback searching for CMake configs
  if (NOT "${OpenCASCADE_DIR}" STREQUAL "")
    set (anOcctDirBak "${OpenCASCADE_DIR}")
    find_package (OpenCASCADE CONFIG QUIET PATHS "${OpenCASCADE_DIR}" NO_DEFAULT_PATH)
    set (OpenCASCADE_DIR "${anOcctDirBak}" CACHE PATH "Path to Open CASCADE libraries." FORCE)
  else()
    find_package (OpenCASCADE CONFIG QUIET)
  endif()
endif()
