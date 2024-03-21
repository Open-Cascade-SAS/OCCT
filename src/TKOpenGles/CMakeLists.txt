project(TKOpenGles)

add_definitions (-DHAVE_GLES2 -DHAVE_EGL)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
#add_definitions("-DOCCT_OPENGL")
