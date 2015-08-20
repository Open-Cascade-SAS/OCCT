project(TKG2d)

set (TOOLKIT_MODULES
  Geom2d
  LProp
  TColGeom2d
  Adaptor2d
  Geom2dLProp
  Geom2dAdaptor
  GProp
)

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
