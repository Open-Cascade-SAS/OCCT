set(OCCT_GeomGridEval_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_GeomGridEval_FILES
  # Common result structures
  GeomGridEval.hxx
  # Curve evaluators
  GeomGridEval_Line.hxx
  GeomGridEval_Circle.hxx
  GeomGridEval_Circle.cxx
  GeomGridEval_Ellipse.hxx
  GeomGridEval_Ellipse.cxx
  GeomGridEval_Hyperbola.hxx
  GeomGridEval_Hyperbola.cxx
  GeomGridEval_Parabola.hxx
  GeomGridEval_Parabola.cxx
  GeomGridEval_BezierCurve.hxx
  GeomGridEval_BezierCurve.cxx
  GeomGridEval_BSplineCurve.hxx
  GeomGridEval_BSplineCurve.cxx
  GeomGridEval_OtherCurve.hxx
  GeomGridEval_OtherCurve.cxx
  GeomGridEval_OffsetCurve.hxx
  GeomGridEval_OffsetCurve.cxx
  GeomGridEval_CurveOnSurface.hxx
  GeomGridEval_CurveOnSurface.cxx
  GeomGridEval_Curve.hxx
  GeomGridEval_Curve.cxx
  # Surface evaluators
  GeomGridEval_Plane.hxx
  GeomGridEval_Cylinder.hxx
  GeomGridEval_Cylinder.cxx
  GeomGridEval_Sphere.hxx
  GeomGridEval_Sphere.cxx
  GeomGridEval_Cone.hxx
  GeomGridEval_Cone.cxx
  GeomGridEval_Torus.hxx
  GeomGridEval_Torus.cxx
  GeomGridEval_BezierSurface.hxx
  GeomGridEval_BezierSurface.cxx
  GeomGridEval_OffsetSurface.hxx
  GeomGridEval_OffsetSurface.cxx
  GeomGridEval_BSplineSurface.hxx
  GeomGridEval_BSplineSurface.cxx
  GeomGridEval_OtherSurface.hxx
  GeomGridEval_OtherSurface.cxx
  GeomGridEval_SurfaceOfRevolution.hxx
  GeomGridEval_SurfaceOfRevolution.cxx
  GeomGridEval_SurfaceOfExtrusion.hxx
  GeomGridEval_SurfaceOfExtrusion.cxx
  GeomGridEval_Surface.hxx
  GeomGridEval_Surface.cxx
)
