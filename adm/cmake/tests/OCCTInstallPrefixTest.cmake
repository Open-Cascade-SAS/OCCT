# Copyright (c) 2026 OPEN CASCADE SAS
# SPDX-License-Identifier: LGPL-2.1-only WITH OCCT-exception-1.0

# cmake -DTEST_DIR=<temporary-directory> -P this-file
cmake_minimum_required(VERSION 3.16)
if(NOT TEST_DIR)
  message(FATAL_ERROR "TEST_DIR is required")
endif()
get_filename_component(OCCT_SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)
file(MAKE_DIRECTORY "${TEST_DIR}/source")
set(_fixture [=[
cmake_minimum_required(VERSION 3.16)
project(OCCTInstallPrefixTest LANGUAGES CXX)
set(OCCT_ROOT_DIR "@OCCT_SOURCE_DIR@")
set(OCCT_BINARY_DIR "${CMAKE_BINARY_DIR}")
set(INSTALL_DIR "${CMAKE_BINARY_DIR}/configured-prefix")
set(CMAKE_INSTALL_PREFIX "${INSTALL_DIR}")
set(INSTALL_DIR_INCLUDE include)
include("${OCCT_ROOT_DIR}/adm/cmake/occt_macros.cmake")
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/Fixture.hxx" "#pragma once\n")
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/Fixture.cxx" "int fixture() { return 1; }\n")
set(OCCT_Fixture_FILES_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}")
set(OCCT_Fixture_FILES Fixture.hxx)
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/include")
COLLECT_AND_INSTALL_OCCT_HEADER_FILES("${CMAKE_BINARY_DIR}" Fixture src include)
set(OCC_VERSION_STRING_EXT fixture)
CONFIGURE_AND_INSTALL_VERSION_HEADER()
set(BUILD_OCCT_VERSION_EXT fixture)
CONFIGURE_AND_INSTALL_VERSION_HEADER()
set(BUILD_USE_PCH ON)
add_library(Fixture STATIC Fixture.cxx)
target_include_directories(Fixture PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
ADD_PRECOMPILED_HEADER(Fixture Fixture.hxx FALSE)
install(TARGETS Fixture EXPORT FixtureTargets ARCHIVE DESTINATION lib)
install(EXPORT FixtureTargets DESTINATION cmake)
]=])
string(CONFIGURE "${_fixture}" _fixture @ONLY)
file(WRITE "${TEST_DIR}/source/CMakeLists.txt" "${_fixture}")
set(_configure_options)
if(TEST_GENERATOR)
  list(APPEND _configure_options -G "${TEST_GENERATOR}")
endif()
if(TEST_CXX_COMPILER)
  list(APPEND _configure_options "-DCMAKE_CXX_COMPILER=${TEST_CXX_COMPILER}")
endif()
if(TEST_GENERATOR_PLATFORM)
  list(APPEND _configure_options -A "${TEST_GENERATOR_PLATFORM}")
endif()
if(TEST_GENERATOR_TOOLSET)
  list(APPEND _configure_options -T "${TEST_GENERATOR_TOOLSET}")
endif()
foreach(_header Fixture.hxx Standard_Version.hxx)
  file(REMOVE "${TEST_DIR}/override-prefix/include/${_header}"
    "${TEST_DIR}/build/configured-prefix/include/${_header}")
endforeach()
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${TEST_DIR}/source" -B "${TEST_DIR}/build"
  -DCMAKE_BUILD_TYPE=Release ${_configure_options}
  RESULT_VARIABLE _status)
if(_status)
  message(FATAL_ERROR "Install-prefix fixture configuration failed")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${TEST_DIR}/build" --config Release
  RESULT_VARIABLE _status)
if(_status)
  message(FATAL_ERROR "Install-prefix fixture build failed")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --install "${TEST_DIR}/build" --config Release
  --prefix "${TEST_DIR}/override-prefix" RESULT_VARIABLE _status)
if(_status)
  message(FATAL_ERROR "Install-prefix fixture installation failed")
endif()
foreach(_header Fixture.hxx Standard_Version.hxx)
  if(NOT EXISTS "${TEST_DIR}/override-prefix/include/${_header}"
     OR EXISTS "${TEST_DIR}/build/configured-prefix/include/${_header}")
    message(FATAL_ERROR "Header did not follow the overridden install prefix: ${_header}")
  endif()
endforeach()
file(READ "${TEST_DIR}/override-prefix/cmake/FixtureTargets.cmake" _exports)
if(_exports MATCHES "INTERFACE_PRECOMPILE_HEADERS")
  message(FATAL_ERROR "Build-only PCH was exported to the installed consumer")
endif()
file(MAKE_DIRECTORY "${TEST_DIR}/consumer")
file(WRITE "${TEST_DIR}/consumer/main.cxx"
  "#include <Fixture.hxx>\nint fixture();\nint main() { return fixture() == 1 ? 0 : 1; }\n")
file(WRITE "${TEST_DIR}/consumer/CMakeLists.txt"
  "cmake_minimum_required(VERSION 3.16)\nproject(InstalledFixture LANGUAGES CXX)\n"
  "include(\"${TEST_DIR}/override-prefix/cmake/FixtureTargets.cmake\")\n"
  "add_executable(consumer main.cxx)\n"
  "target_include_directories(consumer PRIVATE \"${TEST_DIR}/override-prefix/include\")\n"
  "target_link_libraries(consumer PRIVATE Fixture)\n")
execute_process(COMMAND "${CMAKE_COMMAND}" -S "${TEST_DIR}/consumer" -B "${TEST_DIR}/consumer/build"
  -DCMAKE_BUILD_TYPE=Release ${_configure_options} RESULT_VARIABLE _status)
if(_status)
  message(FATAL_ERROR "Installed fixture consumer configuration failed")
endif()
execute_process(COMMAND "${CMAKE_COMMAND}" --build "${TEST_DIR}/consumer/build" --config Release
  RESULT_VARIABLE _status)
if(_status)
  message(FATAL_ERROR "Installed fixture consumer build failed")
endif()
message(STATUS "Install-prefix override, installed consumer and build-only PCH checks passed")

