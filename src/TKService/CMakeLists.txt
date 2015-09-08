project(TKService)

set (TOOLKIT_MODULES
  Aspect
  InterfaceGraphic
  SelectBasics
  Image
  Cocoa
  TColQuantity
  Font
)

if (WIN32)
  list( APPEND TOOLKIT_MODULES "WNT" )
else()
  list( APPEND TOOLKIT_MODULES "Xw" )
endif()

if (WIN32)
  list( APPEND USED_LIBS opengl32.lib )
elseif(APPLE)
  find_library(FRAMEWORKS_OPENGL NAMES OpenGL)
  if(USE_GLX)
    list( APPEND USED_LIBS GL )
    find_package(X11 COMPONENTS X11 Xext Xmu Xi)
    list( APPEND USED_LIBS ${X11_LIBRARIES} )
    list( APPEND USED_LIBS ${X11_Xi_LIB} )
    list( APPEND USED_LIBS ${X11_Xmu_LIB} )
    list( APPEND USED_LIBS Xext )
    list( APPEND USED_LIBS Xmu )
    list( APPEND USED_LIBS Xi )
  else()
    list( APPEND USED_LIBS ${FRAMEWORKS_OPENGL} )
  endif()
  list( APPEND USED_LIBS objc )
  find_library(FRAMEWORKS_APPKIT NAMES Appkit)
  list( APPEND USED_LIBS ${FRAMEWORKS_APPKIT} )
  find_library(FRAMEWORKS_IOKIT NAMES IOKit)
  list( APPEND USED_LIBS ${FRAMEWORKS_IOKIT} )
else()
  list( APPEND USED_LIBS GL )
  list( APPEND USED_LIBS X11 )
  list( APPEND USED_LIBS Xext )
  list( APPEND USED_LIBS Xmu )
  list( APPEND USED_LIBS Xi )
endif()

if (NOT WIN32)
  if(USE_FREEIMAGE)
    list( APPEND USED_LIBS freeimage )
  endif()
  list( APPEND USED_LIBS freetype )
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
