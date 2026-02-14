# Auto-generated list of source files for ExtremaCC package
set(OCCT_ExtremaCC_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_ExtremaCC_FILES
  # Core types and utilities
  ExtremaCC.hxx

  # Distance function for 2D Newton refinement
  ExtremaCC_DistanceFunction.hxx

  # Grid evaluator for 2D parameter space
  ExtremaCC_GridEvaluator2D.hxx

  # Elementary curve wrappers (header-only, for template use)
  ExtremaCC_Circle.hxx
  ExtremaCC_Ellipse.hxx
  ExtremaCC_Hyperbola.hxx
  ExtremaCC_Line.hxx
  ExtremaCC_Parabola.hxx

  # Analytical pair classes (15 pairs)
  ExtremaCC_CircleCircle.hxx
  ExtremaCC_CircleCircle.cxx
  ExtremaCC_CircleEllipse.hxx
  ExtremaCC_CircleEllipse.cxx
  ExtremaCC_CircleHyperbola.hxx
  ExtremaCC_CircleHyperbola.cxx
  ExtremaCC_CircleLine.hxx
  ExtremaCC_CircleLine.cxx
  ExtremaCC_CircleParabola.hxx
  ExtremaCC_CircleParabola.cxx
  ExtremaCC_EllipseEllipse.hxx
  ExtremaCC_EllipseEllipse.cxx
  ExtremaCC_EllipseHyperbola.hxx
  ExtremaCC_EllipseHyperbola.cxx
  ExtremaCC_EllipseLine.hxx
  ExtremaCC_EllipseLine.cxx
  ExtremaCC_EllipseParabola.hxx
  ExtremaCC_EllipseParabola.cxx
  ExtremaCC_HyperbolaHyperbola.hxx
  ExtremaCC_HyperbolaHyperbola.cxx
  ExtremaCC_HyperbolaLine.hxx
  ExtremaCC_HyperbolaLine.cxx
  ExtremaCC_HyperbolaParabola.hxx
  ExtremaCC_HyperbolaParabola.cxx
  ExtremaCC_LineLine.hxx
  ExtremaCC_LineLine.cxx
  ExtremaCC_LineParabola.hxx
  ExtremaCC_LineParabola.cxx
  ExtremaCC_ParabolaParabola.hxx
  ExtremaCC_ParabolaParabola.cxx

  # Curve adapter for numerical evaluation
  ExtremaCC_CurveAdapter.hxx

  # Numerical template for grid-based pairs
  ExtremaCC_Numerical.hxx
  ExtremaCC_Numerical.cxx

  # Main aggregator with std::variant dispatch
  ExtremaCC_Curves.hxx
  ExtremaCC_Curves.cxx
)
