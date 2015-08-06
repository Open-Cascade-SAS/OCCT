project(TKPrim)

set (TOOLKIT_MODULES
  BRepPrim
  BRepSweep
  Sweep
  BRepPrimAPI
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
