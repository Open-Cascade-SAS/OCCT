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

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
