project(TKV3d)

set (TOOLKIT_MODULES
  V3d
  Graphic3d
  Select3D
  Prs3d
  StdPrs
  SelectMgr
  PrsMgr
  AIS
  DsgPrs
  StdSelect
)

if (WIN32)
  list( APPEND USED_LIBS user32.lib )
  list( APPEND USED_LIBS gdi32.lib )
  list( APPEND USED_LIBS opengl32.lib )
elseif(APPLE)
  find_library(FRAMEWORKS_OPENGL NAMES OpenGL)
  if(USE_GLX)
    list( APPEND USED_LIBS GL )
  else()
    list( APPEND USED_LIBS ${FRAMEWORKS_OPENGL} )
  endif()
  list( APPEND USED_LIBS freetype )
else()
  list( APPEND USED_LIBS GL )
  list( APPEND USED_LIBS freetype )
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
