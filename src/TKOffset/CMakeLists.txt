project(TKOffset)

set (TOOLKIT_MODULES
  BRepOffsetAPI
  Draft
  BRepOffset
  BiTgte
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
