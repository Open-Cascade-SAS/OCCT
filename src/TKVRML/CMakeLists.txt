project(TKVRML)

set (TOOLKIT_MODULES
  VrmlConverter
  VrmlAPI
  Vrml
  VrmlData
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
