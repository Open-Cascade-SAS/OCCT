# Source files for GeomHash package
set(OCCT_GeomHash_FILES_LOCATION "${CMAKE_CURRENT_LIST_DIR}")

set(OCCT_GeomHash_FILES
  # Foundational Hashers
  GeomHash_PointHasher.pxx
  GeomHash_DirectionHasher.pxx
  GeomHash_VectorHasher.pxx
  GeomHash_AxisPlacement.pxx

  # Surface Hashers
  GeomHash_PlaneHasher.pxx
  GeomHash_CylindricalSurfaceHasher.pxx
  GeomHash_ConicalSurfaceHasher.pxx
  GeomHash_SphericalSurfaceHasher.pxx
  GeomHash_ToroidalSurfaceHasher.pxx
  GeomHash_SurfaceOfRevolutionHasher.pxx
  GeomHash_SurfaceOfLinearExtrusionHasher.pxx
  GeomHash_BezierSurfaceHasher.pxx
  GeomHash_BSplineSurfaceHasher.pxx
  GeomHash_RectangularTrimmedSurfaceHasher.pxx
  GeomHash_OffsetSurfaceHasher.pxx
  GeomHash_SurfaceHasher.hxx
  GeomHash_SurfaceHasher.cxx

  # Curve Hashers
  GeomHash_LineHasher.pxx
  GeomHash_CircleHasher.pxx
  GeomHash_EllipseHasher.pxx
  GeomHash_HyperbolaHasher.pxx
  GeomHash_ParabolaHasher.pxx
  GeomHash_BezierCurveHasher.pxx
  GeomHash_BSplineCurveHasher.pxx
  GeomHash_TrimmedCurveHasher.pxx
  GeomHash_OffsetCurveHasher.pxx
  GeomHash_CurveHasher.hxx
  GeomHash_CurveHasher.cxx
)
