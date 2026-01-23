# Google Test integration for OCCT toolkits

set (TEST_PROJECT_NAME OpenCascadeGTest)

# Initialize Google Test environment and create the target
function(OCCT_INIT_GTEST)
  if (NOT GOOGLETEST_FOUND)
    message(STATUS "Google Test not available. Skipping test project ${TEST_PROJECT_NAME}")
    return()
  endif()

  # Initialize test data collections
  set(OCCT_GTEST_SOURCE_FILES "" PARENT_SCOPE)
  set(OCCT_GTEST_SOURCE_FILES_ABS "" PARENT_SCOPE)
  set(OCCT_GTEST_TESTS_LIST "" PARENT_SCOPE)

  # Create the test executable once
  add_executable(${TEST_PROJECT_NAME})

  set_target_properties(${TEST_PROJECT_NAME} PROPERTIES FOLDER "Testing")

  # Link with Google Test
  target_link_libraries(${TEST_PROJECT_NAME} PRIVATE GTest::gtest_main)

  # Add pthreads if necessary (for Linux)
  if (UNIX AND NOT APPLE)
    target_link_libraries(${TEST_PROJECT_NAME} PRIVATE pthread)
  endif()

  target_compile_definitions(${TEST_PROJECT_NAME} PRIVATE GTEST_LINKED_AS_SHARED_LIBRARY=1)

  # Configure precompiled headers for Google Test
  if (BUILD_USE_PCH)
    target_precompile_headers(${TEST_PROJECT_NAME} PRIVATE <gtest/gtest.h>)
  endif()

  # Link with all active toolkits that are libraries
  foreach(TOOLKIT ${BUILD_TOOLKITS})
    if(TARGET ${TOOLKIT})
      get_target_property(TOOLKIT_TYPE ${TOOLKIT} TYPE)
      if(TOOLKIT_TYPE STREQUAL "SHARED_LIBRARY" OR TOOLKIT_TYPE STREQUAL "STATIC_LIBRARY")
        target_link_libraries(${TEST_PROJECT_NAME} PRIVATE ${TOOLKIT})
      endif()
    endif()
  endforeach()

  if (INSTALL_GTEST)
    # Install the test executable
    install (TARGETS ${TEST_PROJECT_NAME}
             DESTINATION "${INSTALL_DIR_BIN}\${OCCT_INSTALL_BIN_LETTER}")

    if (EMSCRIPTEN)
      install(FILES ${CMAKE_BINARY_DIR}/${OS_WITH_BIT}/${COMPILER}/bin\${OCCT_INSTALL_BIN_LETTER}/${TEST_PROJECT_NAME}.wasm DESTINATION "${INSTALL_DIR_BIN}/${OCCT_INSTALL_BIN_LETTER}")
    endif()
  endif()
endfunction()

# Add tests from a specific toolkit to the main test executable
function(OCCT_COLLECT_TOOLKIT_TESTS TOOLKIT_NAME)
  # Skip if Google Test is not available or the test executable wasn't created
  if (NOT GOOGLETEST_FOUND OR NOT TARGET ${TEST_PROJECT_NAME})
    return()
  endif()

  # Extract test source files from FILES.cmake
  set(FILES_CMAKE_PATH "${OCCT_${TOOLKIT_NAME}_FILES_LOCATION}/GTests/FILES.cmake")
  if(EXISTS "${FILES_CMAKE_PATH}")
    # Reset toolkit test files list
    set(OCCT_${TOOLKIT_NAME}_GTests_FILES)

    # Include the toolkit's FILES.cmake which sets OCCT_${TOOLKIT_NAME}_GTests_FILES
    include("${FILES_CMAKE_PATH}")
    set(TEST_SOURCE_FILES "${OCCT_${TOOLKIT_NAME}_GTests_FILES}")

    # Skip if no test files found
    if(NOT TEST_SOURCE_FILES)
      return()
    endif()

    # Get module name for test organization
    get_target_property(TOOLKIT_MODULE ${TOOLKIT_NAME} MODULE)
    if(NOT TOOLKIT_MODULE)
      set(TOOLKIT_MODULE "Unknown")
    endif()

    # Get absolute paths of test source files and add them to the executable
    set(TEST_SOURCE_FILES_ABS)
    foreach(TEST_SOURCE_FILE ${TEST_SOURCE_FILES})
      set(TEST_SOURCE_FILE_ABS "${OCCT_${TOOLKIT_NAME}_GTests_FILES_LOCATION}/${TEST_SOURCE_FILE}")
      list(APPEND TEST_SOURCE_FILES_ABS "${TEST_SOURCE_FILE_ABS}")
    endforeach()

    # Add test sources to the executable
    target_sources(${TEST_PROJECT_NAME} PRIVATE ${TEST_SOURCE_FILES_ABS})

    # Create a more reliable test discovery approach
    # Use the WORKING_DIRECTORY parameter to ensure proper test execution context
    gtest_add_tests(
      TARGET ${TEST_PROJECT_NAME}
      TEST_PREFIX "${TOOLKIT_MODULE}::${TOOLKIT_NAME}::"
      SOURCES ${TEST_SOURCE_FILES_ABS}
      TEST_LIST TOOLKIT_TESTS
      SKIP_DEPENDENCY
      WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    )

    # Configure test output properties to improve result capture
    foreach(test_name ${TOOLKIT_TESTS})
      set_tests_properties(${test_name} PROPERTIES
        # Use proper result detection by checking output and return code
        PASS_REGULAR_EXPRESSION "\\[  PASSED  \\]"
        FAIL_REGULAR_EXPRESSION "\\[  FAILED  \\]"
      )
    endforeach()

    # Add these tests to the main list so we can set environment for all tests later
    # Get the existing list first
    if(DEFINED OCCT_GTEST_TESTS_LIST)
      set(TEMP_GTEST_TESTS_LIST ${OCCT_GTEST_TESTS_LIST})
    else()
      set(TEMP_GTEST_TESTS_LIST "")
    endif()
    
    # Append the new tests
    list(APPEND TEMP_GTEST_TESTS_LIST ${TOOLKIT_TESTS})
    
    # Update the parent scope variable
    set(OCCT_GTEST_TESTS_LIST "${TEMP_GTEST_TESTS_LIST}" PARENT_SCOPE)
  endif()
endfunction()

# Set environment variables for all collected tests
function(OCCT_SET_GTEST_ENVIRONMENT)
  if (NOT GOOGLETEST_FOUND OR NOT TARGET ${TEST_PROJECT_NAME})
    return()
  endif()

  if (OCCT_GTEST_TESTS_LIST)
    # Set common environment variables
    set(TEST_ENVIRONMENT
      "CSF_LANGUAGE=us"
      "MMGT_CLEAR=1"
      "CSF_SHMessage=${OCCT_ROOT_DIR}/resources/SHMessage"
      "CSF_MDTVTexturesDirectory=${OCCT_ROOT_DIR}/resources/Textures"
      "CSF_ShadersDirectory=${OCCT_ROOT_DIR}/resources/Shaders"
      "CSF_XSMessage=${OCCT_ROOT_DIR}/resources/XSMessage"
      "CSF_TObjMessage=${OCCT_ROOT_DIR}/resources/TObj"
      "CSF_StandardDefaults=${OCCT_ROOT_DIR}/resources/StdResource"
      "CSF_PluginDefaults=${OCCT_ROOT_DIR}/resources/StdResource"
      "CSF_XCAFDefaults=${OCCT_ROOT_DIR}/resources/StdResource"
      "CSF_TObjDefaults=${OCCT_ROOT_DIR}/resources/StdResource"
      "CSF_StandardLiteDefaults=${OCCT_ROOT_DIR}/resources/StdResource"
      "CSF_IGESDefaults=${OCCT_ROOT_DIR}/resources/XSTEPResource"
      "CSF_STEPDefaults=${OCCT_ROOT_DIR}/resources/XSTEPResource"
      "CSF_XmlOcafResource=${OCCT_ROOT_DIR}/resources/XmlOcafResource"
      "CSF_MIGRATION_TYPES=${OCCT_ROOT_DIR}/resources/StdResource/MigrationSheet.txt"
      "CSF_OCCTResourcePath=${OCCT_ROOT_DIR}/resources"
      "CSF_OCCTDataPath=${OCCT_ROOT_DIR}/data"
      "CSF_OCCTDocPath=${OCCT_ROOT_DIR}/doc"
      "CSF_OCCTTestsPath=${OCCT_ROOT_DIR}/tests"
      "CSF_OCCTBinPath=${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
      "CSF_OCCTLibPath=${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}"
      "CSF_OCCTIncludePath=${CMAKE_BINARY_DIR}/${INSTALL_DIR_INCLUDE}"
      "CASROOT=${OCCT_ROOT_DIR}"
    )

    # Build PATH environment variable
    set(PATH_ELEMENTS
      "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
    )

    # Add 3rdparty paths to PATH
    if(3RDPARTY_TCL_LIBRARY_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_TCL_LIBRARY_DIR}")
    endif()
    if(3RDPARTY_TK_LIBRARY_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_TK_LIBRARY_DIR}")
    endif()
    if(3RDPARTY_FREETYPE_LIBRARY_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_FREETYPE_LIBRARY_DIR}")
    endif()
    if(3RDPARTY_FREEIMAGE_LIBRARY_DIRS)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_FREEIMAGE_LIBRARY_DIRS}")
    endif()
    if(3RDPARTY_TBB_LIBRARY_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_TBB_LIBRARY_DIR}")
    endif()
    if(3RDPARTY_VTK_LIBRARY_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_VTK_LIBRARY_DIR}")
    endif()
    if(3RDPARTY_FFMPEG_LIBRARY_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_FFMPEG_LIBRARY_DIR}")
    endif()
    if(3RDPARTY_QT_DIR)
      list(APPEND PATH_ELEMENTS "${3RDPARTY_QT_DIR}/bin")
    endif()
    if (3RDPARTY_DLL_DIRS)
      foreach(DLL_DIR ${3RDPARTY_DLL_DIRS})
        list(APPEND PATH_ELEMENTS "${DLL_DIR}")
      endforeach()
    endif()

    # Create the PATH variable that ctest will use
    if(WIN32)
      string(REPLACE ";" "\\;" TEST_PATH_ENV "$ENV{PATH}")
      string(REPLACE ";" "\\;" PATH_ELEMENTS_STR "${PATH_ELEMENTS}")
      list(APPEND TEST_ENVIRONMENT "PATH=${PATH_ELEMENTS_STR}\\;${TEST_PATH_ENV}")
    else()
      string(REPLACE ";" ":" PATH_ELEMENTS_STR "${PATH_ELEMENTS}")
      list(APPEND TEST_ENVIRONMENT "PATH=${PATH_ELEMENTS_STR}:$ENV{PATH}")

      # Set LD_LIBRARY_PATH for Unix systems
      list(APPEND TEST_ENVIRONMENT "LD_LIBRARY_PATH=${PATH_ELEMENTS_STR}:$ENV{LD_LIBRARY_PATH}")

      # Set DYLD_LIBRARY_PATH for macOS
      if(APPLE)
        list(APPEND TEST_ENVIRONMENT "DYLD_LIBRARY_PATH=${PATH_ELEMENTS_STR}:$ENV{DYLD_LIBRARY_PATH}")
      endif()
    endif()

    # Add DrawResources related environment if it exists
    if(EXISTS "${OCCT_ROOT_DIR}/resources/DrawResources")
      list(APPEND TEST_ENVIRONMENT "DRAWHOME=${OCCT_ROOT_DIR}/resources/DrawResources")
      list(APPEND TEST_ENVIRONMENT "CSF_DrawPluginDefaults=${OCCT_ROOT_DIR}/resources/DrawResources")

      if(EXISTS "${OCCT_ROOT_DIR}/resources/DrawResources/DrawDefault")
        list(APPEND TEST_ENVIRONMENT "DRAWDEFAULT=${OCCT_ROOT_DIR}/resources/DrawResources/DrawDefault")
      endif()
    endif()

    # Set FPE signal handler if enabled
    if(BUILD_ENABLE_FPE_SIGNAL_HANDLER)
      list(APPEND TEST_ENVIRONMENT "CSF_FPE=1")
    endif()

    # Set TCL/TK library paths if they differ
    if(3RDPARTY_TCL_LIBRARY_DIR AND 3RDPARTY_TK_LIBRARY_DIR AND NOT 3RDPARTY_TCL_LIBRARY_DIR STREQUAL 3RDPARTY_TK_LIBRARY_DIR)
      if(3RDPARTY_TCL_LIBRARY_VERSION_WITH_DOT)
        list(APPEND TEST_ENVIRONMENT "TCL_LIBRARY=${3RDPARTY_TCL_LIBRARY_DIR}/../lib/tcl${3RDPARTY_TCL_LIBRARY_VERSION_WITH_DOT}")
      endif()
      if(3RDPARTY_TK_LIBRARY_VERSION_WITH_DOT)
        list(APPEND TEST_ENVIRONMENT "TK_LIBRARY=${3RDPARTY_TK_LIBRARY_DIR}/../lib/tk${3RDPARTY_TK_LIBRARY_VERSION_WITH_DOT}")
      endif()
    endif()

    # Set environment for all tests in the project
    set_tests_properties(${OCCT_GTEST_TESTS_LIST} PROPERTIES ENVIRONMENT "${TEST_ENVIRONMENT}")
  endif()
endfunction()
