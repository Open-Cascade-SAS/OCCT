include(ExternalProject)

set (GTest_USE_EXTERNAL OFF)
set (GTest_DOWLOAD ON)

find_package(GTest)

if(GTest_FOUND)
    message(STATUS "Found Googletest installation")
    set(GOOGLETEST_FOUND TRUE)
    set(GTest_USE_EXTERNAL TRUE)
else()
    message(STATUS "Googletest not found.")
    set(GOOGLETEST_FOUND FALSE)
    set(GTest_USE_EXTERNAL FALSE)
endif()

if(GTest_USE_EXTERNAL AND NOT GTest_FOUND)
    message(WARNING "GTest_USE_EXTERNAL is ON but GTest not found. Disabling GTest_USE_EXTERNAL.")
    set(GTest_USE_EXTERNAL FALSE)
endif()

if(GTest_DOWLOAD AND GTest_FOUND)
    message(WARNING "GTest_DOWLOAD is ON but GTest found. Disabling GTest_DOWLOAD.")
    set(GTest_DOWLOAD FALSE)
endif()

if(NOT GOOGLETEST_FOUND AND GTest_DOWLOAD)
    message(STATUS "Downloading and building Googletest")
    ExternalProject_Add(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.14.0  # Use a specific tag/release
        SOURCE_DIR     ${CMAKE_BINARY_DIR}/googletest/src
        BINARY_DIR     ${CMAKE_BINARY_DIR}/googletest/build
        INSTALL_DIR    ${CMAKE_BINARY_DIR}/googletest/install
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/googletest/install
            -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        BUILD_COMMAND  ${CMAKE_COMMAND} --build . --config ${CMAKE_BUILD_TYPE}
        INSTALL_COMMAND ${CMAKE_COMMAND} --install . --config ${CMAKE_BUILD_TYPE}
        LOG_DOWNLOAD ON
        LOG_BUILD ON
        LOG_INSTALL ON
    )

    # Create a dummy target to make sure googletest is built before any other target that depends on it
    add_library(gtest_dummy INTERFACE)
    add_dependencies(gtest_dummy googletest)

    # Include directories for the dummy target
    target_include_directories(gtest_dummy INTERFACE
        ${CMAKE_BINARY_DIR}/googletest/install/include
    )
    set(GOOGLETEST_FOUND TRUE)
elseif(GTest_USE_EXTERNAL AND GTest_FOUND)
    message(STATUS "Using external Googletest installation")
else()
    message(WARNING "Googletest is not enabled. Set either GTest_USE_EXTERNAL or GTest_DOWLOAD to enable it.")
    set(GOOGLETEST_FOUND FALSE)
endif()
