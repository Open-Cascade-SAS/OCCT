project(TKOffset)

set (TOOLKIT_MODULES
  BRepOffsetAPI
  Draft
  BRepOffset
  BiTgte
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
