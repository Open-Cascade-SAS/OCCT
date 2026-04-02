# Test source files for TKGeomAlgo
set(OCCT_TKGeomAlgo_GTests_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_TKGeomAlgo_GTests_FILES
  Geom2dAPI_InterCurveCurve_Test.cxx
  Geom2dAPI_PointsToBSpline_Test.cxx
  Geom2dHatch_Elements_Test.cxx
  Geom2dHatch_Intersector_Test.cxx
  GeomAPI_PointsToBSplineSurface_Test.cxx
  GeomAPI_PointsToBSpline_Test.cxx
  Geom2dGcc_Circ2d3Tan_Test.cxx
  GeomFill_CorrectedFrenet_Test.cxx
  GeomFill_Gordon_Test.cxx
  GeomFill_GuideTrihedronAC_Test.cxx
  GeomPlate_BuildPlateSurface_Test.cxx
  Plate_Plate_Test.cxx
  IntCurveSurface_IntersectionPoint_Test.cxx
  IntCurveSurface_InterUtils_Test.cxx
  IntCurveSurface_ThePolyhedronOfHInter_Test.cxx
  IntCurveSurface_ThePolygonOfHInter_Test.cxx
  Intf_Tool_Test.cxx
  IntPatch_Polyhedron_Test.cxx
  IntPolyh_Intersection_Test.cxx
  IntPolyh_Point_Test.cxx
  IntSurf_LineOn2S_Test.cxx
  IntSurf_Quadric_Test.cxx
  TopTrans_SurfaceTransition_Test.cxx
)
