# Source files for Geom2dHash package
set(OCCT_Geom2dHash_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_Geom2dHash_FILES
  # Foundational Hashers
  Geom2dHash_PointHasher.pxx
  Geom2dHash_DirectionHasher.pxx
  Geom2dHash_AxisPlacement.pxx

  # Curve Hashers
  Geom2dHash_LineHasher.pxx
  Geom2dHash_CircleHasher.pxx
  Geom2dHash_EllipseHasher.pxx
  Geom2dHash_HyperbolaHasher.pxx
  Geom2dHash_ParabolaHasher.pxx
  Geom2dHash_BezierCurveHasher.pxx
  Geom2dHash_BSplineCurveHasher.pxx
  Geom2dHash_TrimmedCurveHasher.pxx
  Geom2dHash_OffsetCurveHasher.pxx
  Geom2dHash_CurveHasher.hxx
  Geom2dHash_CurveHasher.cxx
)
