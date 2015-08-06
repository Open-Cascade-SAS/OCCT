project(TKSTL)

set (TOOLKIT_MODULES
  StlMesh
  StlAPI
  StlTransfer
  RWStl
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
