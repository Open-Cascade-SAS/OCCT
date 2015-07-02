project(TKPrim)

set (TOOLKIT_MODULES
  BRepPrim
  BRepSweep
  Sweep
  BRepPrimAPI
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
