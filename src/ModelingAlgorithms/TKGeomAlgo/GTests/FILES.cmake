# Test source files for TKGeomAlgo
set(OCCT_TKGeomAlgo_GTests_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_TKGeomAlgo_GTests_FILES
  Geom2dAPI_InterCurveCurve_Test.cxx
  Geom2dAPI_PointsToBSpline_Test.cxx
  GeomAPI_PointsToBSplineSurface_Test.cxx
  GeomAPI_PointsToBSpline_Test.cxx
  Geom2dGcc_Circ2d3Tan_Test.cxx
  GeomFill_CorrectedFrenet_Test.cxx
  GeomFill_Gordon_Test.cxx
  GeomPlate_BuildPlateSurface_Test.cxx
  IntCurveSurface_ThePolygonOfHInter_Test.cxx
  IntSurf_LineOn2S_Test.cxx
  IntSurf_Quadric_Test.cxx
  TopTrans_SurfaceTransition_Test.cxx
)
