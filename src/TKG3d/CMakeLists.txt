project(TKG3d)

set (TOOLKIT_MODULES
  Geom
  TColGeom
  GeomAdaptor
  AdvApprox
  GeomLProp
  Adaptor3d
  LProp3d
  TopAbs
)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
