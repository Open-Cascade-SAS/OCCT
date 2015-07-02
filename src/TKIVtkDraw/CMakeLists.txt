project(TKIVtkDraw)

set (TOOLKIT_MODULES
  IVtkDraw
)

if (WIN32)
  list( APPEND USED_LIBS ${VTK_LIBRARY_NAMES} )
else()
  foreach(VTK_LIB_NAME ${VTK_LIBRARY_NAMES})
    string(REPLACE ".lib" ".so.1" VTK_LIB_NAME ${VTK_LIB_NAME})
    list( APPEND USED_LIBS "lib${VTK_LIB_NAME}")
  endforeach()
endif()

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
