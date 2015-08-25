project(TKernel)

set (TOOLKIT_MODULES
  FSD
  MMgt
  OSD
  Plugin
  Quantity
  Resource
  Standard
  StdFail
  Storage
  TColStd
  TCollection
  TShort
  Units
  UnitsAPI
  Dico
  NCollection
  Message
)

if (WIN32)
  list( APPEND USED_LIBS advapi32.lib )
  list( APPEND USED_LIBS gdi32.lib )
  list( APPEND USED_LIBS user32.lib )
  list( APPEND USED_LIBS kernel32.lib )
elseif (NOT APPLE)
  list( APPEND USED_LIBS pthread )
  list( APPEND USED_LIBS rt )
endif()

if(USE_TBB)
  list( APPEND USED_LIBS tbb )
  list( APPEND USED_LIBS tbbmalloc )
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
