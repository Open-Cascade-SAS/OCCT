project(TKSTL)

set (TOOLKIT_MODULES
  StlMesh
  StlAPI
  StlTransfer
  RWStl
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
