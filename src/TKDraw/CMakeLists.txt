project(TKDraw)

set (TOOLKIT_MODULES
  Draw
  DBRep
  DrawTrSurf
)

if (WIN32)
  list( APPEND USED_LIBS gdi32.lib )
  list( APPEND USED_LIBS advapi32.lib )
  list( APPEND USED_LIBS user32.lib )
elseif(APPLE)
  find_library(FRAMEWORKS_TCL NAMES Tcl)
  list( APPEND USED_LIBS ${FRAMEWORKS_TCL} )
  find_library(FRAMEWORKS_TK NAMES Tk)
  list( APPEND USED_LIBS ${FRAMEWORKS_TK} )
  list( APPEND USED_LIBS objc )
  find_library(FRAMEWORKS_APPKIT NAMES Appkit)
  list( APPEND USED_LIBS ${FRAMEWORKS_APPKIT} )
  find_library(FRAMEWORKS_IOKIT NAMES IOKit)
  list( APPEND USED_LIBS ${FRAMEWORKS_IOKIT} )
else()
  list( APPEND USED_LIBS tcl8.6 )
  list( APPEND USED_LIBS X11 )
  list( APPEND USED_LIBS tk8.6 )
endif()

if(USE_TBB)
  list( APPEND USED_LIBS tbb )
  list( APPEND USED_LIBS tbbmalloc )
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
