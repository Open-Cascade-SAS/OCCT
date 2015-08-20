project(TKGeomBase)

set (TOOLKIT_MODULES
  ProjLib
  GeomProjLib
  GCPnts
  CPnts
  Approx
  AppParCurves
  FEmTool
  AppCont
  Extrema
  IntAna
  IntAna2d
  GeomConvert
  AdvApp2Var
  GeomLib
  Geom2dConvert
  Hermit
  BndLib
  AppDef
  GeomTools
  GC
  GCE2d
  gce
)

include(${OCCT_SOURCE_DIR}/adm/cmake/occt_toolkit.cmake)
