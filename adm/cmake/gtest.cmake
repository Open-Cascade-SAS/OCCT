# Google Test integration for OCCT

# Only proceed if tests are enabled
if (NOT BUILD_GTEST)
  set(GOOGLETEST_FOUND FALSE)
  return()
endif()

# Check if the user has specified whether to install Google Test
if (NOT DEFINED INSTALL_GTEST)
  set(INSTALL_GTEST OFF CACHE BOOL "Install Google Test")
endif()

# Google Test configuration options
option(GTEST_USE_EXTERNAL "Use externally provided Google Test installation" OFF)
option(GTEST_USE_FETCHCONTENT "Use FetchContent to download and build Google Test" ON)

# Try to find existing GTest installation
find_package(GTest QUIET)

if(GTest_FOUND)
  message(STATUS "Found Googletest installation")
  set(GOOGLETEST_FOUND TRUE)
  set(GTEST_USE_EXTERNAL TRUE)
  set(GTEST_USE_FETCHCONTENT FALSE)
else()
  message(STATUS "Googletest not found in system paths")
  if(GTEST_USE_FETCHCONTENT)
    include(FetchContent)

    # Set option to disable GMock before declaring the content
    set(BUILD_GMOCK OFF CACHE BOOL "Builds the googlemock subproject" FORCE)

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.16.0.zip
      DOWNLOAD_EXTRACT_TIMESTAMP true
    )

    FetchContent_MakeAvailable(googletest)

    # Set proper grouping for the targets in solution explorer
    if(TARGET gtest)
      set_target_properties(gtest PROPERTIES FOLDER "ThirdParty/GoogleTest")
      target_compile_definitions(gtest PRIVATE GTEST_CREATE_SHARED_LIBRARY=1)
    endif()
    if(TARGET gtest_main)
      set_target_properties(gtest_main PROPERTIES FOLDER "ThirdParty/GoogleTest")
      target_compile_definitions(gtest_main PRIVATE GTEST_CREATE_SHARED_LIBRARY=1)
    endif()

    # Set variables for consistent use throughout the build system
    set(GOOGLETEST_FOUND TRUE)
  else()
    message(STATUS "Google Test not available. Tests will be skipped.")
    set(GOOGLETEST_FOUND FALSE)
  endif()
endif()

# Enable CTest if Google Test is available
if(GOOGLETEST_FOUND)
  include(GoogleTest)
endif()
