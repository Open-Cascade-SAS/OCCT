# Auto-generated list of source files for ExtremaPC package
set(OCCT_ExtremaPC_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_ExtremaPC_FILES
  # Core types
  ExtremaPC.hxx
  ExtremaPC_POnCurv.hxx

  # Elementary curves (header-only, analytical solutions)
  ExtremaPC_Line.hxx
  ExtremaPC_Circle.hxx
  ExtremaPC_Ellipse.hxx
  ExtremaPC_Hyperbola.hxx
  ExtremaPC_Parabola.hxx

  # Grid-based infrastructure for numerical curves
  ExtremaPC_DistanceFunction.hxx
  ExtremaPC_GridEvaluator.hxx

  # Numerical curve evaluators (grid-based)
  ExtremaPC_BezierCurve.hxx
  ExtremaPC_BezierCurve.cxx
  ExtremaPC_BSplineCurve.hxx
  ExtremaPC_BSplineCurve.cxx
  ExtremaPC_OffsetCurve.hxx
  ExtremaPC_OffsetCurve.cxx
  ExtremaPC_OtherCurve.hxx
  ExtremaPC_OtherCurve.cxx

  # Main aggregator with std::variant dispatch
  ExtremaPC_Curve.hxx
  ExtremaPC_Curve.cxx
)
