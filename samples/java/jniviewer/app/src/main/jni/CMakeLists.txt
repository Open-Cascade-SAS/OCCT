cmake_minimum_required(VERSION 3.4.1)

set(HEADER_FILES OcctJni_MsgPrinter.hxx OcctJni_Viewer.hxx)
set(SOURCE_FILES OcctJni_MsgPrinter.cxx OcctJni_Viewer.cxx)

set (anOcctLibs
  TKernel TKMath TKG2d TKG3d TKGeomBase TKBRep TKGeomAlgo TKTopAlgo TKShHealing TKMesh
  # exchange
  TKPrim TKBO TKBool TKFillet TKOffset
  TKXSBase
  TKDEIGES
  TKDESTEP
  # OCCT Visualization
  TKService TKHLR TKV3d TKOpenGles
)

set(aLibDeps "")

# OCCT libraries
include_directories(${OCCT_ROOT}/inc)
foreach(anOcctLib ${anOcctLibs})
  add_library(lib_${anOcctLib} SHARED IMPORTED)
  set_target_properties(lib_${anOcctLib} PROPERTIES IMPORTED_LOCATION ${OCCT_ROOT}/libs/${ANDROID_ABI}/lib${anOcctLib}.so)
  list(APPEND aLibDeps lib_${anOcctLib})
endforeach()

# FreeType
add_library(lib_FreeType SHARED IMPORTED)
set_target_properties(lib_FreeType PROPERTIES IMPORTED_LOCATION ${FREETYPE_ROOT}/libs/${ANDROID_ABI}/libfreetype.so)
list(APPEND aLibDeps lib_FreeType)

# FreeImage - uncomment, if OCCT was built with FreeImage
#add_library(lib_FreeImage SHARED IMPORTED)
#set_target_properties(lib_FreeImage PROPERTIES IMPORTED_LOCATION ${FREETYPE_ROOT}/libs/${ANDROID_ABI}/libfreeimage.so)
#list(APPEND aLibDeps lib_FreeImage)

# system libraries
list(APPEND aLibDeps EGL GLESv2 log android)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -frtti -fexceptions -fpermissive")

add_library(TKJniSample SHARED ${SOURCE_FILES})
target_link_libraries(TKJniSample ${aLibDeps})
