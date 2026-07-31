# OpenVR SDK
# OpenVR is an API and runtime that allows access to VR hardware from multiple vendors without requiring that applications have specific knowledge of the hardware they are targeting.
# https://github.com/ValveSoftware/openvr

# vcpkg processing
if (BUILD_USE_VCPKG)
  set (USED_3RDPARTY_OPENVR_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
  set (CMAKE_FIND_LIBRARY_SUFFIXES .lib .so .dylib .a)
  find_library (FIND_OPENVR_LIB NAMES "${CSF_OpenVR}"
                PATHS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib"
                CMAKE_FIND_ROOT_PATH_BOTH
                NO_DEFAULT_PATH)
  set (CSF_OpenVR "${FIND_OPENVR_LIB}")
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/")
  return()
endif()

THIRDPARTY_PRODUCT("OPENVR" "openvr.h" "CSF_OpenVR" "")
