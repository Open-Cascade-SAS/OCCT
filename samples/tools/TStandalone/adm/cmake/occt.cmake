#freeimage

if(NOT "${CASROOT}" STREQUAL "")
  #message(FATAL_ERROR, "Empty OCCT dir")
  #return()
  #set("3RDPARTY_OCCT_DIR ${CASROOT}")
endif()

#set(3RDPARTY_OCC_DIR ${CASROOT})
#message("3RDPARTY_OCC_DIR: ${3RDPARTY_OCC_DIR}")
#set("3RDPARTY_OCC_DIR ${3RDPARTY_OCCT_DIR}")
#message("3RDPARTY_OCC_DIR: ${3RDPARTY_OCC_DIR}")

set(CSF_TKernel "TKernel")
THIRDPARTY_PRODUCT("OCCT" "Standard.hxx" "CSF_TKernel" "")

set(3RDPARTY_OCCT_DIR ${3RDPARTY_OCC_DIR} CACHE PATH "OCCT dir")

OCCT_MAKE_OS_WITH_BITNESS()
OCCT_MAKE_COMPILER_SHORT_NAME()

SET(3RDPARTY_OCCT_INCLUDE_DIR "${3RDPARTY_OCC_INCLUDE_DIR}" CACHE PATH "OCCT include dir")
SET(3RDPARTY_OCCT_LIBRARY_DIR "${3RDPARTY_OCC_LIBRARY_DIR}" CACHE PATH "OCCT library dir")
SET(3RDPARTY_OCCT_DLL_DIR "${3RDPARTY_OCC_DLL_DIR}" CACHE PATH "OCCT dll dir")

SET(CSF_StandardDefaults_DIR ${CASROOT}/src/StdResource CACHE PATH "OCCT Standard Resource")
SET(CSF_XCAFDefaults_DIR ${CASROOT}/src/StdResource CACHE PATH "OCCT XCAF Defaults")
SET(CSF_PluginDefaults_DIR ${CASROOT}/src/StdResource CACHE PATH "OCCT Plugin Defaults")

message (STATUS "... OCCT dir: ${3RDPARTY_OCCT_DIR}")
message (STATUS "... OCCT Include dirs: ${3RDPARTY_OCCT_INCLUDE_DIR}")
message (STATUS "... OCCT Library dirs: ${3RDPARTY_OCCT_LIBRARY_DIRS}")
message (STATUS "... OCCT Binary  dirs: ${3RDPARTY_OCCT_DLL_DIRS}")

SET(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} "${3RDPARTY_OCCT_LIBRARY_DIRS}")

find_library(TKernel TKernel PATHS "${3RDPARTY_OCCT_LIBRARY_DIRS}"
                             PATH_SUFFIXES lib
                             NO_DEFAULT_PATH)

if ("${TKernel}" STREQUAL "TKernel-NOTFOUND")
  set (OCCT_LIBRARY_NOT_FOUND ON)
endif()

find_library(TKMath TKMath)

find_library(TKCAF TKCAF)
find_library(TKCDF TKCDF)
find_library(TKLCAF TKLCAF)
find_library(TKVCAF TKVCAF)

find_library(TKBRep TKBRep)
find_library(TKService TKService)

find_library(TKOpenGl TKOpenGl)
find_library(TKV3d TKV3d)

find_library(TKXCAF TKXCAF)
find_library(TKXSBase TKXSBase)
find_library(TKXDESTEP TKXDESTEP)
find_library(TKXmlXCAF TKXmlXCAF)
find_library(TKXml TKXml)

find_library(TKBin TKBin)
find_library(TKBinL TKBinL)
find_library(TKBinXCAF TKBinXCAF)

find_library(TKStd TKStd)
find_library(TKXmlL TKXmlL)
find_library(TKStdL TKStdL)
# VInspector additionally:
find_library(TKBO TKBO)
# ShapeView additionally:
find_library(TKGeomBase TKGeomBase)
find_library(TKG3d TKG3d)
