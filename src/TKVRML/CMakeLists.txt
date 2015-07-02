project(TKVRML)

set (TOOLKIT_MODULES
  VrmlConverter
  VrmlAPI
  Vrml
  VrmlData
)

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
