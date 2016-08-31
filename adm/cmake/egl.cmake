# EGL
if (WIN32)
  THIRDPARTY_PRODUCT("EGL" "EGL/egl.h" "libEGL" "libEGL")
else()
  THIRDPARTY_PRODUCT("EGL" "EGL/egl.h" "EGL" "EGL")
endif()
