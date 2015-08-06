project(TKMesh)

set (TOOLKIT_MODULES
  BRepMesh
)

if(NOT WIN32)
  if(USE_TBB)
    list( APPEND USED_LIBS tbb )
    list( APPEND USED_LIBS tbbmalloc )
  endif()
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
