# Get git information for ABI validation
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/../.."
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET
    RESULT_VARIABLE GIT_RESULT
)

if(NOT GIT_RESULT EQUAL 0)
    set(GIT_COMMIT_HASH "unknown")
endif()

set(SOURCE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../../..")

if (VCPKG_LIBRARY_LINKAGE STREQUAL "dynamic")
    set(BUILD_TYPE "Shared")
else()
    set(BUILD_TYPE "Static")
endif()

set (BUILD_CPP_STANDARD "C++17")
if ("cpp20" IN_LIST FEATURES)
    set (BUILD_CPP_STANDARD "C++20")
elseif ("cpp23" IN_LIST FEATURES)
    set (BUILD_CPP_STANDARD "C++23")
endif()

set (BUILD_OPT_PROFILE "Default")
if ("optimized" IN_LIST FEATURES)
    set (BUILD_OPT_PROFILE "Production")
endif()

set (BUILD_MODULE_Draw OFF)
if ("tcl" IN_LIST FEATURES OR "tcltk" IN_LIST FEATURES)
    set (BUILD_MODULE_Draw ON)
endif()

set (USE_MMGR_TYPE "NATIVE")
if ("jemalloc" IN_LIST FEATURES)
    set (USE_MMGR_TYPE "JEMALLOC")
endif()

# Feature mapping to CMake options
vcpkg_check_features(OUT_FEATURE_OPTIONS FEATURE_OPTIONS
    FEATURES
        angle       USE_GLES2
        opengl      USE_OPENGL
        freeimage   USE_FREEIMAGE
        freetype    USE_FREETYPE
        rapidjson   USE_RAPIDJSON
        tbb         USE_TBB
        vtk         USE_VTK
        tcltk       USE_TK
        draco       USE_DRACO
        ffmpeg      USE_FFMPEG
        openvr      USE_OPENVR
        gtest       BUILD_GTEST
        pch         BUILD_USE_PCH
        d3d         USE_D3D
        docs        BUILD_DOC_Overview
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        ${FEATURE_OPTIONS}
        -DBUILD_LIBRARY_TYPE=${BUILD_TYPE}
        -DINSTALL_DIR_LAYOUT=Vcpkg
        -DBUILD_USE_VCPKG=ON
        -DBUILD_CPP_STANDARD=${BUILD_CPP_STANDARD}
        -DBUILD_OPT_PROFILE=${BUILD_OPT_PROFILE}
        -DBUILD_MODULE_Draw=${BUILD_MODULE_Draw}
        -DCMAKE_INSTALL_PREFIX=${CURRENT_PACKAGES_DIR}
    MAYBE_UNUSED_VARIABLES
        -DUSE_HASH=${GIT_COMMIT_HASH}
)

vcpkg_cmake_install()

vcpkg_install_copyright(
    FILE_LIST
        "${SOURCE_PATH}/LICENSE_LGPL_21.txt"
        "${SOURCE_PATH}/OCCT_LGPL_EXCEPTION.txt"
)
