// Copyright (c) 2025 OPEN CASCADE SAS
//
// This file is part of Open CASCADE Technology software library.
//
// This library is free software; you can redistribute it and/or modify it under
// the terms of the GNU Lesser General Public License version 2.1 as published
// by the Free Software Foundation, with special exception defined in the file
// OCCT_LGPL_EXCEPTION.txt. Consult the file LICENSE_LGPL_21.txt included in OCCT
// distribution for complete text of the license and disclaimer of any warranty.
//
// Alternatively, this file may be used under the terms of Open CASCADE
// commercial license or contractual agreement.

// Unit tests for GeomProp free functions and result structures.

#include <Geom_BezierCurve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_Circle.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <GeomAbs_Shape.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomProp.hxx>
#include <GeomProp_Curve.hxx>
#include <GeomProp_Surface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Ax3.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>
#include <NCollection_Array1.hxx>
#include <Precision.hxx>

#include <cmath>

#include <gtest/gtest.h>

// ============================================================================
// Free function tests - ComputeTangent
// ============================================================================

TEST(GeomPropTest, ComputeTangent_D1NonZero)
{
  const gp_Vec                  aD1(1.0, 0.0, 0.0);
  const gp_Vec                  aD2(0.0, 1.0, 0.0);
  const gp_Vec                  aD3(0.0, 0.0, 1.0);
  const GeomProp::TangentResult aRes =
    GeomProp::ComputeTangent(aD1, aD2, aD3, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), 1.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Y(), 0.0, Precision::Confusion());
  EXPECT_NEAR(aRes.Direction.Z(), 0.0, Precision::Confusion());
}

TEST(GeomPropTest, ComputeTangent_D1Zero_D2NonZero)
{
  const gp_Vec                  aD1(0.0, 0.0, 0.0);
  const gp_Vec                  aD2(0.0, 1.0, 0.0);
  const gp_Vec                  aD3(0.0, 0.0, 1.0);
  const GeomProp::TangentResult aRes =
    GeomProp::ComputeTangent(aD1, aD2, aD3, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.Y(), 1.0, Precision::Confusion());
}

TEST(GeomPropTest, ComputeTangent_AllZero)
{
  const gp_Vec                  aD1(0.0, 0.0, 0.0);
  const gp_Vec                  aD2(0.0, 0.0, 0.0);
  const gp_Vec                  aD3(0.0, 0.0, 0.0);
  const GeomProp::TangentResult aRes =
    GeomProp::ComputeTangent(aD1, aD2, aD3, Precision::Confusion());
  EXPECT_FALSE(aRes.IsDefined);
}

// ============================================================================
// Free function tests - ComputeCurvature
// ============================================================================

TEST(GeomPropTest, ComputeCurvature_CircularArc)
{
  // At (1,0,0) on unit circle in XY plane: D1=(0,1,0), D2=(-1,0,0)
  const gp_Vec                    aD1(0.0, 1.0, 0.0);
  const gp_Vec                    aD2(-1.0, 0.0, 0.0);
  const GeomProp::CurvatureResult aRes =
    GeomProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_FALSE(aRes.IsInfinite);
  EXPECT_NEAR(aRes.Value, 1.0, 1.0e-10);
}

TEST(GeomPropTest, ComputeCurvature_ZeroD1)
{
  const gp_Vec                    aD1(0.0, 0.0, 0.0);
  const gp_Vec                    aD2(1.0, 0.0, 0.0);
  const GeomProp::CurvatureResult aRes =
    GeomProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  EXPECT_TRUE(aRes.IsDefined);
  EXPECT_TRUE(aRes.IsInfinite);
}

TEST(GeomPropTest, ComputeCurvature_StraightLine)
{
  const gp_Vec                    aD1(1.0, 0.0, 0.0);
  const gp_Vec                    aD2(0.0, 0.0, 0.0);
  const GeomProp::CurvatureResult aRes =
    GeomProp::ComputeCurvature(aD1, aD2, Precision::Confusion());
  EXPECT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Value, 0.0, Precision::Confusion());
}

// ============================================================================
// Free function tests - ComputeNormal
// ============================================================================

TEST(GeomPropTest, ComputeNormal_CircularArc)
{
  const gp_Vec                 aD1(0.0, 1.0, 0.0);
  const gp_Vec                 aD2(-1.0, 0.0, 0.0);
  const GeomProp::NormalResult aRes = GeomProp::ComputeNormal(aD1, aD2, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.Direction.X(), -1.0, Precision::Confusion());
}

// ============================================================================
// Free function tests - ComputeSurfaceNormal
// ============================================================================

TEST(GeomPropTest, ComputeSurfaceNormal_XYPlane)
{
  const gp_Vec                        aD1U(1.0, 0.0, 0.0);
  const gp_Vec                        aD1V(0.0, 1.0, 0.0);
  const GeomProp::SurfaceNormalResult aRes =
    GeomProp::ComputeSurfaceNormal(aD1U, aD1V, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(std::abs(aRes.Direction.Z()), 1.0, Precision::Confusion());
}

TEST(GeomPropTest, ComputeSurfaceNormal_DegeneratePoint)
{
  const gp_Vec                        aD1U(0.0, 0.0, 0.0);
  const gp_Vec                        aD1V(0.0, 1.0, 0.0);
  const GeomProp::SurfaceNormalResult aRes =
    GeomProp::ComputeSurfaceNormal(aD1U, aD1V, Precision::Confusion());
  EXPECT_FALSE(aRes.IsDefined);
}

// ============================================================================
// Free function tests - ComputeMeanGaussian
// ============================================================================

TEST(GeomPropTest, ComputeMeanGaussian_Sphere)
{
  // At north pole of unit sphere, D1U and D1V are orthogonal unit vectors
  const gp_Vec                       aD1U(1.0, 0.0, 0.0);
  const gp_Vec                       aD1V(0.0, 1.0, 0.0);
  const gp_Vec                       aD2U(0.0, 0.0, -1.0);
  const gp_Vec                       aD2V(0.0, 0.0, -1.0);
  const gp_Vec                       aDUV(0.0, 0.0, 0.0);
  const GeomProp::MeanGaussianResult aRes =
    GeomProp::ComputeMeanGaussian(aD1U, aD1V, aD2U, aD2V, aDUV, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.MeanCurvature, -1.0, 1.0e-10);
  EXPECT_NEAR(aRes.GaussianCurvature, 1.0, 1.0e-10);
}

// ============================================================================
// GeomProp_Curve - initialization and basic queries
// ============================================================================

TEST(GeomPropCurveTest, Line_ZeroCurvature)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Line);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(0.5, Precision::Confusion());
  EXPECT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 0.0, Precision::Confusion());
}

TEST(GeomPropCurveTest, Circle_ConstantCurvature)
{
  const double             aRadius = 5.0;
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aRadius);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);

  GeomProp_Curve aProp(aCircle);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Circle);

  const GeomProp::CurvatureResult aCurv = aProp.Curvature(1.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(aCurv.Value, 1.0 / aRadius, 1.0e-10);
}

TEST(GeomPropCurveTest, Ellipse_CurvatureExtrema)
{
  const double              aMajor = 10.0;
  const double              aMinor = 5.0;
  gp_Elips                  anElips(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), aMajor, aMinor);
  occ::handle<Geom_Ellipse> anEllipse = new Geom_Ellipse(anElips);

  GeomProp_Curve aProp(anEllipse);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 4);
}

TEST(GeomPropCurveTest, Hyperbola_SingleExtremum)
{
  gp_Hypr                     anHypr(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 6.0, 3.0);
  occ::handle<Geom_Hyperbola> aHyperbola = new Geom_Hyperbola(anHypr);

  GeomProp_Curve aProp(aHyperbola);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 1);
  EXPECT_NEAR(aResult.Points[0].Parameter, 0.0, Precision::Confusion());
}

TEST(GeomPropCurveTest, Parabola_SingleExtremum)
{
  gp_Parab                   aParab(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  occ::handle<Geom_Parabola> aParabola = new Geom_Parabola(aParab);

  GeomProp_Curve aProp(aParabola);

  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 1);
  EXPECT_NEAR(aResult.Points[0].Parameter, 0.0, Precision::Confusion());
}

TEST(GeomPropCurveTest, Circle_NoExtrema)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);

  GeomProp_Curve                aProp(aCircle);
  const GeomProp::CurveAnalysis aResult = aProp.FindCurvatureExtrema();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomPropCurveTest, Circle_NoInflections)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);

  GeomProp_Curve                aProp(aCircle);
  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_EQ(aResult.Points.Length(), 0);
}

TEST(GeomPropCurveTest, BezierCurve_Inflections)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1)                             = gp_Pnt(0, 0, 0);
  aPoles(2)                             = gp_Pnt(1, 2, 0);
  aPoles(3)                             = gp_Pnt(3, -1, 0);
  aPoles(4)                             = gp_Pnt(4, 1, 0);
  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);

  GeomProp_Curve aProp(aBezier);
  EXPECT_EQ(aProp.GetType(), GeomAbs_BezierCurve);

  const GeomProp::CurveAnalysis aResult = aProp.FindInflections();
  ASSERT_TRUE(aResult.IsDone);
  EXPECT_GE(aResult.Points.Length(), 1);
}

TEST(GeomPropCurveTest, Line_TangentDirection)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(0, 1, 0));
  GeomProp_Curve         aProp(aLine);

  const GeomProp::TangentResult aTan = aProp.Tangent(5.0, Precision::Confusion());
  ASSERT_TRUE(aTan.IsDefined);
  EXPECT_NEAR(aTan.Direction.Y(), 1.0, Precision::Confusion());
}

TEST(GeomPropCurveTest, Circle_Normal)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);

  GeomProp_Curve aProp(aCircle);

  // At param=0, point is (5,0,0), normal should point toward center (-1,0,0)
  const GeomProp::NormalResult aNorm = aProp.Normal(0.0, Precision::Confusion());
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(aNorm.Direction.X(), -1.0, 1.0e-6);
}

TEST(GeomPropCurveTest, Circle_CentreOfCurvature)
{
  gp_Circ                  aCirc(gp_Ax2(gp_Pnt(1, 2, 3), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_Circle> aCircle = new Geom_Circle(aCirc);

  GeomProp_Curve aProp(aCircle);

  const GeomProp::CentreResult aCentre = aProp.CentreOfCurvature(0.0, Precision::Confusion());
  ASSERT_TRUE(aCentre.IsDefined);
  EXPECT_NEAR(aCentre.Centre.X(), 1.0, 1.0e-6);
  EXPECT_NEAR(aCentre.Centre.Y(), 2.0, 1.0e-6);
  EXPECT_NEAR(aCentre.Centre.Z(), 3.0, 1.0e-6);
}

// ============================================================================
// GeomProp_Surface - initialization and basic queries
// ============================================================================

TEST(GeomPropSurfaceTest, Plane_ZeroCurvatures)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());

  GeomProp_Surface aProp(aPlane);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Plane);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.0, 0.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_TRUE(aCurv.IsUmbilic);
  EXPECT_NEAR(aCurv.MinCurvature, 0.0, Precision::Confusion());
  EXPECT_NEAR(aCurv.MaxCurvature, 0.0, Precision::Confusion());
}

TEST(GeomPropSurfaceTest, Plane_Normal)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());

  GeomProp_Surface aProp(aPlane);

  const GeomProp::SurfaceNormalResult aNorm = aProp.Normal(0.0, 0.0, Precision::Confusion());
  ASSERT_TRUE(aNorm.IsDefined);
  EXPECT_NEAR(std::abs(aNorm.Direction.Z()), 1.0, Precision::Confusion());
}

TEST(GeomPropSurfaceTest, Plane_MeanGaussian)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());

  GeomProp_Surface aProp(aPlane);

  const GeomProp::MeanGaussianResult aRes = aProp.MeanGaussian(0.0, 0.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(aRes.MeanCurvature, 0.0, Precision::Confusion());
  EXPECT_NEAR(aRes.GaussianCurvature, 0.0, Precision::Confusion());
}

TEST(GeomPropSurfaceTest, Sphere_ConstantCurvature)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);

  GeomProp_Surface aProp(aSphere);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Sphere);

  // Curvature sign depends on normal orientation. For outward-pointing normal,
  // convex surfaces have negative curvature (center on opposite side of normal).
  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 0.5, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  EXPECT_NEAR(std::abs(aCurv.MinCurvature), 1.0 / aRadius, 1.0e-10);
  EXPECT_NEAR(std::abs(aCurv.MaxCurvature), 1.0 / aRadius, 1.0e-10);
}

TEST(GeomPropSurfaceTest, Sphere_MeanGaussian)
{
  const double                       aRadius = 5.0;
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), aRadius);

  GeomProp_Surface aProp(aSphere);

  const GeomProp::MeanGaussianResult aRes = aProp.MeanGaussian(0.5, 0.5, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(std::abs(aRes.MeanCurvature), 1.0 / aRadius, 1.0e-10);
  EXPECT_NEAR(aRes.GaussianCurvature, 1.0 / (aRadius * aRadius), 1.0e-10);
}

TEST(GeomPropSurfaceTest, Cylinder_Curvatures)
{
  const double                         aRadius = 3.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);

  GeomProp_Surface aProp(aCyl);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Cylinder);

  const GeomProp::SurfaceCurvatureResult aCurv = aProp.Curvatures(0.5, 1.0, Precision::Confusion());
  ASSERT_TRUE(aCurv.IsDefined);
  // One curvature is zero (along axis), the other is non-zero (1/R with sign from normal).
  const double aAbsMin = std::abs(aCurv.MinCurvature);
  const double aAbsMax = std::abs(aCurv.MaxCurvature);
  EXPECT_TRUE(aAbsMin < 1.0e-10 || aAbsMax < 1.0e-10);
  EXPECT_NEAR(std::max(aAbsMin, aAbsMax), 1.0 / aRadius, 1.0e-10);
}

TEST(GeomPropSurfaceTest, Cylinder_MeanGaussian)
{
  const double                         aRadius = 3.0;
  occ::handle<Geom_CylindricalSurface> aCyl    = new Geom_CylindricalSurface(gp_Ax3(), aRadius);

  GeomProp_Surface aProp(aCyl);

  const GeomProp::MeanGaussianResult aRes = aProp.MeanGaussian(0.5, 1.0, Precision::Confusion());
  ASSERT_TRUE(aRes.IsDefined);
  EXPECT_NEAR(std::abs(aRes.MeanCurvature), 1.0 / (2.0 * aRadius), 1.0e-10);
  EXPECT_NEAR(aRes.GaussianCurvature, 0.0, Precision::Confusion());
}

TEST(GeomPropSurfaceTest, Cone_CurvaturesVaryAlongV)
{
  gp_Ax3                           anAx3;
  occ::handle<Geom_ConicalSurface> aCone = new Geom_ConicalSurface(anAx3, M_PI / 6.0, 5.0);

  GeomProp_Surface aProp(aCone);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Cone);

  const GeomProp::SurfaceCurvatureResult aCurv1 =
    aProp.Curvatures(0.5, 0.0, Precision::Confusion());
  const GeomProp::SurfaceCurvatureResult aCurv2 =
    aProp.Curvatures(0.5, 5.0, Precision::Confusion());
  ASSERT_TRUE(aCurv1.IsDefined);
  ASSERT_TRUE(aCurv2.IsDefined);
  // One curvature is zero (along ruling), the other varies with V.
  // The magnitude of the non-zero curvature should decrease with V (larger radius).
  const double aNonZero1 = std::max(std::abs(aCurv1.MinCurvature), std::abs(aCurv1.MaxCurvature));
  const double aNonZero2 = std::max(std::abs(aCurv2.MinCurvature), std::abs(aCurv2.MaxCurvature));
  EXPECT_GT(aNonZero1, aNonZero2);
}

TEST(GeomPropSurfaceTest, Torus_CurvaturesVaryAlongV)
{
  const double                      aMajor = 10.0;
  const double                      aMinor = 3.0;
  occ::handle<Geom_ToroidalSurface> aTorus = new Geom_ToroidalSurface(gp_Ax3(), aMajor, aMinor);

  GeomProp_Surface aProp(aTorus);
  EXPECT_EQ(aProp.GetType(), GeomAbs_Torus);

  // At V=0 (outer edge): k1=1/r, k2=1/(R+r)
  const GeomProp::MeanGaussianResult aRes0 = aProp.MeanGaussian(0.0, 0.0, Precision::Confusion());
  ASSERT_TRUE(aRes0.IsDefined);
  EXPECT_GT(aRes0.GaussianCurvature, 0.0); // Both curvatures positive at outer edge

  // At V=PI (inner edge): k1=1/r, k2=-1/(R-r) (negative)
  const GeomProp::MeanGaussianResult aResPI = aProp.MeanGaussian(0.0, M_PI, Precision::Confusion());
  ASSERT_TRUE(aResPI.IsDefined);
  EXPECT_LT(aResPI.GaussianCurvature, 0.0); // Negative Gaussian curvature at inner edge
}

// ============================================================================
// Geometry() / Adaptor() accessor tests
// ============================================================================

TEST(GeomPropCurveTest, Geometry_HandlePath_ReturnsNonNull)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(), gp_Dir(1, 0, 0));
  GeomProp_Curve         aProp(aLine);
  EXPECT_NE(aProp.Geometry(), nullptr);
  EXPECT_EQ(aProp.Adaptor(), nullptr);
}

TEST(GeomPropCurveTest, Geometry_AdaptorPath_Line_ReturnsNonNull)
{
  occ::handle<Geom_Line> aLine = new Geom_Line(gp_Pnt(), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aLine);
  GeomProp_Curve         aProp(anAdaptor);
  // Line evaluator extracts geometry from adaptor
  EXPECT_NE(aProp.Geometry(), nullptr);
}

TEST(GeomPropCurveTest, Geometry_TrimmedCurve_ReturnsBasisCurve)
{
  occ::handle<Geom_Circle>       aCircle  = new Geom_Circle(gp_Ax2(), 5.0);
  occ::handle<Geom_TrimmedCurve> aTrimmed = new Geom_TrimmedCurve(aCircle, 0.0, M_PI);
  GeomProp_Curve                 aProp(aTrimmed);

  const Geom_Curve* aGeom = aProp.Geometry();
  ASSERT_NE(aGeom, nullptr);
  // The geometry should be the unwrapped basis circle, not the trimmed curve.
  EXPECT_TRUE(aGeom->IsKind(STANDARD_TYPE(Geom_Circle)));
}

TEST(GeomPropCurveTest, Geometry_AllCurveTypes_NonNull)
{
  // Line
  {
    occ::handle<Geom_Line> aCurve = new Geom_Line(gp_Pnt(), gp_Dir(0, 0, 1));
    GeomProp_Curve         aProp(aCurve);
    EXPECT_NE(aProp.Geometry(), nullptr);
  }
  // Ellipse
  {
    occ::handle<Geom_Ellipse> aCurve = new Geom_Ellipse(gp_Ax2(), 5.0, 3.0);
    GeomProp_Curve            aProp(aCurve);
    EXPECT_NE(aProp.Geometry(), nullptr);
  }
  // Hyperbola
  {
    occ::handle<Geom_Hyperbola> aCurve = new Geom_Hyperbola(gp_Ax2(), 5.0, 3.0);
    GeomProp_Curve              aProp(aCurve);
    EXPECT_NE(aProp.Geometry(), nullptr);
  }
  // Parabola
  {
    occ::handle<Geom_Parabola> aCurve = new Geom_Parabola(gp_Ax2(), 2.0);
    GeomProp_Curve             aProp(aCurve);
    EXPECT_NE(aProp.Geometry(), nullptr);
  }
}

TEST(GeomPropSurfaceTest, Geometry_HandlePath_ReturnsNonNull)
{
  occ::handle<Geom_Plane> aPlane = new Geom_Plane(gp_Ax3());
  GeomProp_Surface        aProp(aPlane);
  EXPECT_NE(aProp.Geometry(), nullptr);
  EXPECT_EQ(aProp.Adaptor(), nullptr);
}

TEST(GeomPropSurfaceTest, Geometry_AdaptorPath_ReturnsNull)
{
  // On the adaptor path, most evaluators do not extract the geometry handle
  occ::handle<Geom_SphericalSurface> aSphere = new Geom_SphericalSurface(gp_Ax3(), 5.0);
  GeomAdaptor_Surface                anAdaptor(aSphere);
  GeomProp_Surface                   aProp(anAdaptor);
  EXPECT_EQ(aProp.Geometry(), nullptr);
}

TEST(GeomPropCurveTest, Continuity_TwoLines_C1)
{
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(1, 0, 0));
  // Junction at u1=1 (point (1,0,0)) and u2=0 (point (1,0,0))
  GeomAbs_Shape aCont = GeomProp_Curve::Continuity(aL1, aL2, 1.0, 0.0, false, false);
  EXPECT_GE(aCont, GeomAbs_C1);
}

TEST(GeomPropCurveTest, Continuity_ReversedDirection_G1)
{
  occ::handle<Geom_Line> aL1 = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  occ::handle<Geom_Line> aL2 = new Geom_Line(gp_Pnt(1, 0, 0), gp_Dir(-1, 0, 0));
  // Same point, reversed tangent vectors — only G1 with reversal flags
  GeomAbs_Shape aCont = GeomProp_Curve::Continuity(aL1, aL2, 1.0, 0.0, false, true);
  EXPECT_GE(aCont, GeomAbs_C1);
}
