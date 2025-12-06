# Test source files for TKG3d
set(OCCT_TKG3d_GTests_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_TKG3d_GTests_FILES
  Geom_BSplineCurve_Test.cxx
  Geom_BezierCurve_Test.cxx
  Geom_BezierSurface_Test.cxx
  Geom_BSplineSurface_Test.cxx
  Geom_OffsetCurve_Test.cxx
  Geom_OffsetSurface_Test.cxx
  GeomAdaptor_Curve_Test.cxx
  GeomAPI_ExtremaCurveCurve_Test.cxx
  GeomAPI_Interpolate_Test.cxx
  GeomHash_CurveHasher_Test.cxx
  GeomHash_SurfaceHasher_Test.cxx
)
