project(TKBRep)

set (TOOLKIT_MODULES
  TopoDS
  TopExp
  TopTools
  BRep
  BRepLProp
  BRepAdaptor
  BRepTools
  BinTools
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
