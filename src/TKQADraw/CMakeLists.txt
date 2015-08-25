project(TKQADraw)

set (TOOLKIT_MODULES
  QABugs
  QADNaming
  QADraw
  QANCollection
  QANewBRepNaming
  QANewDBRepNaming
  QANewModTopOpe
)

if (WIN32)
  list( APPEND USED_LIBS gdi32.lib )
  list( APPEND USED_LIBS advapi32.lib )
  list( APPEND USED_LIBS user32.lib )
endif()

if(USE_TBB)
  list( APPEND USED_LIBS tbb )
  list( APPEND USED_LIBS tbbmalloc )
endif()

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
