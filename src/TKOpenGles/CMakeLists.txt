project(TKOpenGles)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)

if (APPLE AND BUILD_USE_PCH)
  set_target_properties(TKOpenGles PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)
endif()
