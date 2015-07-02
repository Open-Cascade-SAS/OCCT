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

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
