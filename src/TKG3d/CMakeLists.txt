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

include(${OCCT_SOURCE_DIR}/adm/cmake/BuildToolkit.cmake)
