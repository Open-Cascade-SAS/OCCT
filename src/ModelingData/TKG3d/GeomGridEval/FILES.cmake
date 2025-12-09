set(OCCT_GeomGridEval_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_GeomGridEval_FILES
  # Common result structures
  GeomGridEval.hxx
  # Curve evaluators
  GeomGridEval_Line.hxx
  GeomGridEval_Circle.hxx
  GeomGridEval_Circle.cxx
  GeomGridEval_BSplineCurve.hxx
  GeomGridEval_BSplineCurve.cxx
  GeomGridEval_OtherCurve.hxx
  GeomGridEval_OtherCurve.cxx
  GeomGridEval_Curve.hxx
  GeomGridEval_Curve.cxx
  # Surface evaluators
  GeomGridEval_Plane.hxx
  GeomGridEval_Cylinder.hxx
  GeomGridEval_Cylinder.cxx
  GeomGridEval_Sphere.hxx
  GeomGridEval_Sphere.cxx
  GeomGridEval_BSplineSurface.hxx
  GeomGridEval_BSplineSurface.cxx
  GeomGridEval_OtherSurface.hxx
  GeomGridEval_OtherSurface.cxx
  GeomGridEval_Surface.hxx
  GeomGridEval_Surface.cxx
)
