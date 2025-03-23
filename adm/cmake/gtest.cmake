# Google Test integration for OCCT

# Only proceed if tests are enabled
if (NOT BUILD_GTEST)
  set(GOOGLETEST_FOUND FALSE)
  return()
endif()

# Check if the user has specified whether to install Google Test
if (NOT DEFINED INSTALL_GTEST)
  set(INSTALL_GTEST CACHE BOOL "Install Google Test" OFF)
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
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
      DOWNLOAD_EXTRACT_TIMESTAMP true
    )

    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)

    # Set proper grouping for the targets in solution explorer
    if(TARGET gtest)
      set_target_properties(gtest PROPERTIES FOLDER "ThirdParty/GoogleTest")
    endif()
    if(TARGET gtest_main)
      set_target_properties(gtest_main PROPERTIES FOLDER "ThirdParty/GoogleTest")
    endif()
    if(TARGET gmock)
      set_target_properties(gmock PROPERTIES FOLDER "ThirdParty/GoogleTest")
    endif()
    if(TARGET gmock_main)
      set_target_properties(gmock_main PROPERTIES FOLDER "ThirdParty/GoogleTest")
    endif()

    # Set variables for consistent use throughout the build system
    set(GOOGLETEST_FOUND TRUE)
    set(GTEST_INCLUDE_DIRS ${googletest_SOURCE_DIR}/googletest/include)
    set(GTEST_LIBRARIES gtest)
    set(GTEST_MAIN_LIBRARIES gtest_main)
    set(GTEST_BOTH_LIBRARIES ${GTEST_LIBRARIES} ${GTEST_MAIN_LIBRARIES})
  else()
    message(STATUS "Google Test not available. Tests will be skipped.")
    set(GOOGLETEST_FOUND FALSE)
  endif()
endif()

# Enable CTest if Google Test is available
if(GOOGLETEST_FOUND)
  include(GoogleTest)
endif()
