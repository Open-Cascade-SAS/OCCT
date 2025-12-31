# Auto-generated list of source files for ExtremaCS package
set(OCCT_ExtremaCS_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_ExtremaCS_FILES
  # Core types and utilities
  ExtremaCS.hxx

  # Grid evaluator for 3D parameter space (t, u, v)
  ExtremaCS_GridEvaluator3D.hxx

  # Elementary curve wrappers (header-only)
  ExtremaCS_Line.hxx
  ExtremaCS_Circle.hxx
  ExtremaCS_Ellipse.hxx
  ExtremaCS_Hyperbola.hxx
  ExtremaCS_Parabola.hxx

  # Numerical curve evaluators
  ExtremaCS_BezierCurve.hxx
  ExtremaCS_BSplineCurve.hxx
  ExtremaCS_OffsetCurve.hxx
  ExtremaCS_OtherCurve.hxx

  # Elementary surface wrappers (header-only)
  ExtremaCS_Plane.hxx
  ExtremaCS_Cylinder.hxx
  ExtremaCS_Cone.hxx
  ExtremaCS_Sphere.hxx
  ExtremaCS_Torus.hxx

  # Numerical surface evaluators
  ExtremaCS_BezierSurface.hxx
  ExtremaCS_BSplineSurface.hxx
  ExtremaCS_OffsetSurface.hxx
  ExtremaCS_SurfaceOfRevolution.hxx
  ExtremaCS_SurfaceOfExtrusion.hxx
  ExtremaCS_OtherSurface.hxx

  # Analytical pair classes (closed-form solutions)
  ExtremaCS_LinePlane.hxx
  ExtremaCS_LineSphere.hxx
  ExtremaCS_LineCylinder.hxx
  ExtremaCS_LineCone.hxx
  ExtremaCS_CirclePlane.hxx
  ExtremaCS_CircleSphere.hxx
  ExtremaCS_CircleCylinder.hxx
  ExtremaCS_EllipsePlane.hxx

  # Numerical template for grid-based pairs
  ExtremaCS_Numerical.hxx

  # Main aggregator with std::variant dispatch
  ExtremaCS_CurveSurface.hxx
  ExtremaCS_CurveSurface.cxx
)
