project(TKBO)

set (TOOLKIT_MODULES
  IntTools
  BRepAlgoAPI
  BOPCol
  BOPDS
  BOPAlgo
  BOPTools
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
