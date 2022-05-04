cmake_minimum_required(VERSION 3.2)

project(glfw-occt-demo)

set (CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/adm/cmake" ${CMAKE_MODULE_PATH})

set(APP_VERSION_MAJOR 1)
set(APP_VERSION_MINOR 0)
set(APP_TARGET glfwocct)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
file(GLOB SOURCES
  *.h
  *.cpp
)
source_group ("Headers" FILES
  GlfwOcctView.h
  GlfwOcctWindow.h)
source_group ("Sources" FILES
  GlfwOcctView.cpp
  GlfwOcctWindow.cpp
  main.cpp)

# OpenGL
find_package(OpenGL REQUIRED)

# Open CASCADE Technology
find_package (OpenCASCADE REQUIRED)
if (NOT OpenCASCADE_FOUND)
  message (FATAL_ERROR "coult not find OpenCASCADE, please set OpenCASCADE_DIR variable" )
else()
  message (STATUS "Using OpenCASCADE from \"${OpenCASCADE_INSTALL_PREFIX}\"" )
  message (STATUS "OpenCASCADE_INCLUDE_DIR=${OpenCASCADE_INCLUDE_DIR}")
  message (STATUS "OpenCASCADE_LIBRARY_DIR=${OpenCASCADE_LIBRARY_DIR}")
  INCLUDE_DIRECTORIES(${OpenCASCADE_INCLUDE_DIR})
endif()

SET(OpenCASCADE_LIBS 
  TKernel
  TKService
  TKV3d
  TKOpenGl
  TKBRep
  TKGeomBase
  TKGeomAlgo
  TKG3d
  TKG2d
  TKTopAlgo
  TKPrim
)

# glfw
find_package(glfw3 REQUIRED)
if (glfw3_FOUND)
  message (STATUS "Using glfw3 ${glfw3_VERSION}" )
  INCLUDE_DIRECTORIES(${GLFW_INCLUDE_DIRS})
  LINK_DIRECTORIES(${GLFW_LIBRARY_DIRS})
else()
  message (STATUS "glfw3 is not found." )
endif()

add_executable(${APP_TARGET} ${SOURCES})
target_link_libraries(
  ${APP_TARGET}
  ${OpenCASCADE_LIBS}
  glfw
  ${OPENGL_LIBRARIES}
)
