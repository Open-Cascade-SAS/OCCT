#freeimage

# vcpkg processing
if (BUILD_USE_VCPKG)
find_package(freeimage CONFIG REQUIRED)
  set(CSF_FreeImagePlus freeimage::FreeImagePlus)
  if (WIN32)
    set (USED_3RDPARTY_FREEIMAGE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin")
  else()
    set (USED_3RDPARTY_FREEIMAGE_DIR "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/lib")
  endif()
  list (APPEND 3RDPARTY_INCLUDE_DIRS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include/")
  return()
endif()

THIRDPARTY_PRODUCT("FREEIMAGE" "FreeImage.h" "CSF_FreeImagePlus" "d")
