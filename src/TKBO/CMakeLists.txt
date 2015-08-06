project(TKBO)

set (TOOLKIT_MODULES
  IntTools
  BRepAlgoAPI
  BOPCol
  BOPDS
  BOPAlgo
  BOPTools
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
