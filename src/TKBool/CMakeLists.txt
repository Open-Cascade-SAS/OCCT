project(TKBool)

set (TOOLKIT_MODULES
  TopOpeBRep
  TopOpeBRepDS
  TopOpeBRepBuild
  TopOpeBRepTool
  BRepAlgo
  BRepFill
  BRepProj
)

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
