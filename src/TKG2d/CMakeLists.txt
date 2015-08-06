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

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
