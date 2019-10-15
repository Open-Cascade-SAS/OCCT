cmake_minimum_required(VERSION 3.2)

project(occt-webgl-sample)

set(CMAKE_CXX_STANDARD 11)
set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_TARGET occt-webgl-sample)

# customize build
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s WASM=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s USE_WEBGL2=1")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ALLOW_MEMORY_GROWTH=1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s SAFE_HEAP=1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s NO_EXIT_RUNTIME=1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s TOTAL_MEMORY=16MB")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s ABORTING_MALLOC=0")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s FORCE_FILESYSTEM=1")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --preload-file myFile")

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
file(GLOB SOURCES
  *.h
  *.cpp
)
source_group ("Headers" FILES
  WasmOcctView.h)
source_group ("Sources" FILES
  WasmOcctView.cpp
  main.cpp)

# FreeType
find_package(freetype REQUIRED NO_DEFAULT_PATH)
if(freetype_FOUND)
  message (STATUS "Using FreeType from \"${freetype_DIR}\"" )
else()
  message(WARNING "Could not find FreeType, please set freetype_DIR variable." )
endif()

# Open CASCADE Technology
find_package(OpenCASCADE REQUIRED NO_DEFAULT_PATH)
if(OpenCASCADE_FOUND)
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_DIR}\"" )
  INCLUDE_DIRECTORIES(${OpenCASCADE_INCLUDE_DIR})
  LINK_DIRECTORIES(${OpenCASCADE_LIBRARY_DIR})
else()
  message(WARNING "Could not find OpenCASCADE, please set OpenCASCADE_DIR variable." )
  set(OCCT_LIBRARY_DIR)
  set(OCCT_BIN_DIR)
endif()

set(OpenCASCADE_LIBS TKRWMesh TKBinXCAF TKBin TKBinL TKOpenGl TKXCAF TKVCAF TKCAF TKV3d TKHLR TKMesh TKService TKShHealing TKPrim TKTopAlgo TKGeomAlgo TKBRep TKGeomBase TKG3d TKG2d TKMath TKLCAF TKCDF TKernel)

add_executable(${APP_TARGET} ${SOURCES})
target_link_libraries(
  ${APP_TARGET}
  ${OpenCASCADE_LIBS}
  freetype
)
set_target_properties(${APP_TARGET} PROPERTIES LINK_FLAGS "-s EXPORTED_FUNCTIONS=['_main','_onFileDataRead'] -s EXTRA_EXPORTED_RUNTIME_METHODS=['ccall','cwrap']")

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}" LIBRARY DESTINATION "${CMAKE_INSTALL_PREFIX}")
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.wasm DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES occt-webgl-sample.html DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${OpenCASCADE_RESOURCE_DIR}/DrawResources/OCC_logo.png DESTINATION ${CMAKE_INSTALL_PREFIX})
install(FILES ${OpenCASCADE_RESOURCE_DIR}/DrawResources/lamp.ico DESTINATION ${CMAKE_INSTALL_PREFIX})
