#freeimageplus

if (WIN32)
  if (3RDPARTY_FREEIMAGE_DIR AND NOT 3RDPARTY_FREEIMAGEPLUS_DIR)
    set (3RDPARTY_FREEIMAGEPLUS_DIR "${3RDPARTY_FREEIMAGE_DIR}" CACHE PATH "The directory containing freeimageplus" FORCE)
  endif()

  THIRDPARTY_PRODUCT("FREEIMAGEPLUS" "FreeImagePlus.h" "freeimageplus" "freeimageplusd")
endif()