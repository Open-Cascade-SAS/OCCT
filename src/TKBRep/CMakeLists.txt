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

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
