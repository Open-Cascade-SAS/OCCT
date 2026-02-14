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

#include <gtest/gtest.h>

#include <ExtremaSS_GenericPair.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_SphericalSurface.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>

//! Test fixture for GenericPair surface-surface extrema tests.
class ExtremaSS_GenericPairTest : public testing::Test
{
protected:
  static constexpr double THE_TOL = 1e-5;

  //! Create a simple bilinear Bezier surface (flat plane-like).
  Handle(Geom_BezierSurface) createBezierPlane(const gp_Pnt& theCorner,
                                               double        theSizeU,
                                               double        theSizeV,
                                               const gp_Dir& theNormal)
  {
    gp_Dir aDirU, aDirV;
    if (std::abs(theNormal.Z()) < 0.9)
    {
      aDirU = gp_Dir(0, 0, 1).Crossed(theNormal);
    }
    else
    {
      aDirU = gp_Dir(1, 0, 0).Crossed(theNormal);
    }
    aDirV = theNormal.Crossed(aDirU);

    NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
    aPoles(1, 1) = theCorner;
    aPoles(2, 1) = theCorner.Translated(gp_Vec(aDirU) * theSizeU);
    aPoles(1, 2) = theCorner.Translated(gp_Vec(aDirV) * theSizeV);
    aPoles(2, 2) = theCorner.Translated(gp_Vec(aDirU) * theSizeU + gp_Vec(aDirV) * theSizeV);

    return new Geom_BezierSurface(aPoles);
  }

  //! Create a curved Bezier surface (saddle shape).
  Handle(Geom_BezierSurface) createSaddleSurface(const gp_Pnt& theCenter, double theSize)
  {
    NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);

    // Create a saddle-shaped surface: z = x^2 - y^2 centered at theCenter
    for (int i = 1; i <= 3; ++i)
    {
      for (int j = 1; j <= 3; ++j)
      {
        double aU = (i - 2) * theSize; // -1, 0, 1 scaled
        double aV = (j - 2) * theSize;
        double aZ = aU * aU - aV * aV; // Saddle shape

        aPoles(i, j) = gp_Pnt(theCenter.X() + aU, theCenter.Y() + aV, theCenter.Z() + aZ);
      }
    }

    return new Geom_BezierSurface(aPoles);
  }

  //! Create a cubic BSpline surface (wavy surface).
  //! Uses 4x4 control points for cubic degree (Bezier-like segment).
  Handle(Geom_BSplineSurface) createBSplineWavySurface(const gp_Pnt& theCenter,
                                                       double        theSizeU,
                                                       double        theSizeV,
                                                       double        theAmplitude)
  {
    // 4x4 control points for a cubic BSpline (Bezier-like)
    const int aNbPoles = 4;
    const int aDegree  = 3;

    NCollection_Array2<gp_Pnt> aPoles(1, aNbPoles, 1, aNbPoles);

    for (int i = 1; i <= aNbPoles; ++i)
    {
      for (int j = 1; j <= aNbPoles; ++j)
      {
        double aU = (i - 1) * theSizeU / (aNbPoles - 1) - theSizeU / 2.0;
        double aV = (j - 1) * theSizeV / (aNbPoles - 1) - theSizeV / 2.0;
        // Wavy pattern using control point displacement
        double aZ = theAmplitude * std::sin(M_PI * (i - 1) / (aNbPoles - 1))
                    * std::cos(M_PI * (j - 1) / (aNbPoles - 1));

        aPoles(i, j) = gp_Pnt(theCenter.X() + aU, theCenter.Y() + aV, theCenter.Z() + aZ);
      }
    }

    // Clamped BSpline: 2 knots, each with multiplicity = degree + 1 = 4
    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = aDegree + 1; // 4
    aMults(2) = aDegree + 1; // 4

    return new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, aDegree, aDegree);
  }

  //! Create a BSpline plane-like surface.
  Handle(Geom_BSplineSurface) createBSplinePlane(const gp_Pnt& theCorner,
                                                 double        theSizeU,
                                                 double        theSizeV,
                                                 const gp_Dir& theNormal)
  {
    gp_Dir aDirU, aDirV;
    if (std::abs(theNormal.Z()) < 0.9)
    {
      aDirU = gp_Dir(0, 0, 1).Crossed(theNormal);
    }
    else
    {
      aDirU = gp_Dir(1, 0, 0).Crossed(theNormal);
    }
    aDirV = theNormal.Crossed(aDirU);

    // 4x4 control points for cubic BSpline
    const int                  aNbPoles = 4;
    NCollection_Array2<gp_Pnt> aPoles(1, aNbPoles, 1, aNbPoles);

    for (int i = 1; i <= aNbPoles; ++i)
    {
      for (int j = 1; j <= aNbPoles; ++j)
      {
        double aU    = (i - 1) * theSizeU / (aNbPoles - 1);
        double aV    = (j - 1) * theSizeV / (aNbPoles - 1);
        aPoles(i, j) = theCorner.Translated(gp_Vec(aDirU) * aU + gp_Vec(aDirV) * aV);
      }
    }

    const int aDegree = 3;

    NCollection_Array1<double> aKnots(1, 2);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;

    NCollection_Array1<int> aMults(1, 2);
    aMults(1) = aNbPoles;
    aMults(2) = aNbPoles;

    return new Geom_BSplineSurface(aPoles, aKnots, aKnots, aMults, aMults, aDegree, aDegree);
  }
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaSS_GenericPairTest, TwoBezierPlanes_Parallel)
{
  // Two parallel Bezier plane-like surfaces separated by distance 5
  Handle(Geom_BezierSurface) aSurf1 = createBezierPlane(gp_Pnt(0, 0, 0), 10, 10, gp_Dir(0, 0, 1));
  Handle(Geom_BezierSurface) aSurf2 = createBezierPlane(gp_Pnt(0, 0, 5), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(aSurf1);
  GeomAdaptor_Surface anAdaptor2(aSurf2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  // Should find extrema (for parallel planes, all points are at same distance)
  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 5
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_GenericPairTest, TwoBezierPlanes_Perpendicular)
{
  // Two perpendicular Bezier plane-like surfaces that intersect
  Handle(Geom_BezierSurface) aSurf1 = createBezierPlane(gp_Pnt(-5, -5, 0), 10, 10, gp_Dir(0, 0, 1));
  Handle(Geom_BezierSurface) aSurf2 = createBezierPlane(gp_Pnt(0, -5, -5), 10, 10, gp_Dir(1, 0, 0));

  GeomAdaptor_Surface anAdaptor1(aSurf1);
  GeomAdaptor_Surface anAdaptor2(aSurf2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // They intersect, so minimum distance should be zero or very small
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 0.0, 0.5);
}

TEST_F(ExtremaSS_GenericPairTest, SaddleSurface_VsPlane)
{
  // A saddle surface and a flat Bezier plane
  Handle(Geom_BezierSurface) aSaddle = createSaddleSurface(gp_Pnt(0, 0, 5), 2.0);
  Handle(Geom_BezierSurface) aPlane = createBezierPlane(gp_Pnt(-5, -5, 0), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(aSaddle);
  GeomAdaptor_Surface anAdaptor2(aPlane);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Bezier surface from saddle-shaped poles approximates z=x^2-y^2 but doesn't interpolate exactly.
  // The minimum distance between surfaces should be positive and reasonable.
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 0.0);  // Should be positive distance
  EXPECT_LT(aMinDist, 10.0); // Should be reasonable (saddle center is at z=5)
}

TEST_F(ExtremaSS_GenericPairTest, TwoSaddleSurfaces_Separated)
{
  // Two saddle surfaces at different positions
  Handle(Geom_BezierSurface) aSaddle1 = createSaddleSurface(gp_Pnt(0, 0, 0), 2.0);
  Handle(Geom_BezierSurface) aSaddle2 = createSaddleSurface(gp_Pnt(10, 0, 0), 2.0);

  GeomAdaptor_Surface anAdaptor1(aSaddle1);
  GeomAdaptor_Surface anAdaptor2(aSaddle2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Surfaces are separated by ~6 units (center to center is 10, minus 2*2 for extent)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 4.0);
  EXPECT_LT(aMinDist, 8.0);
}

TEST_F(ExtremaSS_GenericPairTest, WithDomain_RestrictedSearch)
{
  // Two Bezier surfaces with restricted domain
  Handle(Geom_BezierSurface) aSurf1 = createBezierPlane(gp_Pnt(0, 0, 0), 10, 10, gp_Dir(0, 0, 1));
  Handle(Geom_BezierSurface) aSurf2 = createBezierPlane(gp_Pnt(0, 0, 5), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(aSurf1);
  GeomAdaptor_Surface anAdaptor2(aSurf2);

  // Create restricted domain (only first quarter of each surface)
  ExtremaSS::Domain4D aDomain;
  aDomain.Domain1.UMin = 0.0;
  aDomain.Domain1.UMax = 0.5;
  aDomain.Domain1.VMin = 0.0;
  aDomain.Domain1.VMax = 0.5;
  aDomain.Domain2.UMin = 0.0;
  aDomain.Domain2.UMax = 0.5;
  aDomain.Domain2.VMin = 0.0;
  aDomain.Domain2.VMax = 0.5;

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2, aDomain);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should still be 5 (parallel surfaces)
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 5.0, THE_TOL);

  // Verify result parameters are within domain
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    const ExtremaSS::ExtremumResult& anExt = aResult[i];
    EXPECT_GE(anExt.U1, -THE_TOL);
    EXPECT_LE(anExt.U1, 0.5 + THE_TOL);
    EXPECT_GE(anExt.V1, -THE_TOL);
    EXPECT_LE(anExt.V1, 0.5 + THE_TOL);
    EXPECT_GE(anExt.U2, -THE_TOL);
    EXPECT_LE(anExt.U2, 0.5 + THE_TOL);
    EXPECT_GE(anExt.V2, -THE_TOL);
    EXPECT_LE(anExt.V2, 0.5 + THE_TOL);
  }
}

TEST_F(ExtremaSS_GenericPairTest, PerformWithBoundary_IncludesCorners)
{
  // Two separated Bezier surfaces
  Handle(Geom_BezierSurface) aSurf1 = createBezierPlane(gp_Pnt(0, 0, 0), 10, 10, gp_Dir(0, 0, 1));
  Handle(Geom_BezierSurface) aSurf2 = createBezierPlane(gp_Pnt(20, 0, 0), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(aSurf1);
  GeomAdaptor_Surface anAdaptor2(aSurf2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.PerformWithBoundary(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be 10 (closest edges)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 10.0, 1.0);
}

TEST_F(ExtremaSS_GenericPairTest, SearchModeMin_OnlyFindsMinimum)
{
  // Two saddle surfaces
  Handle(Geom_BezierSurface) aSaddle1 = createSaddleSurface(gp_Pnt(0, 0, 0), 2.0);
  Handle(Geom_BezierSurface) aSaddle2 = createSaddleSurface(gp_Pnt(6, 0, 0), 2.0);

  GeomAdaptor_Surface anAdaptor1(aSaddle1);
  GeomAdaptor_Surface anAdaptor2(aSaddle2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL, ExtremaSS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // All found extrema should be minima
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    EXPECT_TRUE(aResult[i].IsMinimum);
  }
}

TEST_F(ExtremaSS_GenericPairTest, SwappedOrder_SameResult)
{
  // Two saddle surfaces
  Handle(Geom_BezierSurface) aSaddle1 = createSaddleSurface(gp_Pnt(0, 0, 0), 2.0);
  Handle(Geom_BezierSurface) aSaddle2 = createSaddleSurface(gp_Pnt(6, 0, 0), 2.0);

  GeomAdaptor_Surface anAdaptor1(aSaddle1);
  GeomAdaptor_Surface anAdaptor2(aSaddle2);

  ExtremaSS_GenericPair    anEval1(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult1 = anEval1.Perform(THE_TOL);

  ExtremaSS_GenericPair    anEval2(anAdaptor2, anAdaptor1);
  const ExtremaSS::Result& aResult2 = anEval2.Perform(THE_TOL);

  ASSERT_EQ(aResult1.Status, ExtremaSS::Status::OK);
  ASSERT_EQ(aResult2.Status, ExtremaSS::Status::OK);

  // Distance should be the same regardless of order
  EXPECT_NEAR(aResult1.MinSquareDistance(), aResult2.MinSquareDistance(), THE_TOL * THE_TOL);
}

//==================================================================================================
// BSpline Surface Tests
//==================================================================================================

TEST_F(ExtremaSS_GenericPairTest, TwoBSplinePlanes_Parallel)
{
  // Two parallel BSpline plane-like surfaces separated by distance 5
  Handle(Geom_BSplineSurface) aSurf1 =
    createBSplinePlane(gp_Pnt(-5, -5, 0), 10, 10, gp_Dir(0, 0, 1));
  Handle(Geom_BSplineSurface) aSurf2 =
    createBSplinePlane(gp_Pnt(-5, -5, 5), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(aSurf1);
  GeomAdaptor_Surface anAdaptor2(aSurf2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Distance should be 5
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), 5.0, THE_TOL);
}

TEST_F(ExtremaSS_GenericPairTest, BSplineWavy_VsBSplinePlane)
{
  // A wavy BSpline surface above a flat BSpline plane
  Handle(Geom_BSplineSurface) aWavy =
    createBSplineWavySurface(gp_Pnt(0, 0, 5), 8.0, 8.0, 1.0); // amplitude 1
  Handle(Geom_BSplineSurface) aPlane =
    createBSplinePlane(gp_Pnt(-5, -5, 0), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(aWavy);
  GeomAdaptor_Surface anAdaptor2(aPlane);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Minimum distance should be ~4 (wavy surface center at z=5, with amplitude 1 going down)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 3.0);
  EXPECT_LT(aMinDist, 6.0);
}

TEST_F(ExtremaSS_GenericPairTest, TwoBSplineWavy_Surfaces)
{
  // Two wavy BSpline surfaces at different heights
  Handle(Geom_BSplineSurface) aWavy1 = createBSplineWavySurface(gp_Pnt(0, 0, 0), 8.0, 8.0, 1.0);
  Handle(Geom_BSplineSurface) aWavy2 = createBSplineWavySurface(gp_Pnt(0, 0, 6), 8.0, 8.0, 1.0);

  GeomAdaptor_Surface anAdaptor1(aWavy1);
  GeomAdaptor_Surface anAdaptor2(aWavy2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Surfaces are 6 units apart at center, each has amplitude 1
  // Minimum distance ~4 (when both waves are closest)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 2.0);
  EXPECT_LT(aMinDist, 8.0);
}

TEST_F(ExtremaSS_GenericPairTest, BSpline_VsBezier_Mixed)
{
  // BSpline wavy surface vs Bezier saddle surface
  Handle(Geom_BSplineSurface) aBSpline = createBSplineWavySurface(gp_Pnt(0, 0, 0), 6.0, 6.0, 0.5);
  Handle(Geom_BezierSurface)  aBezier  = createSaddleSurface(gp_Pnt(10, 0, 0), 2.0);

  GeomAdaptor_Surface anAdaptor1(aBSpline);
  GeomAdaptor_Surface anAdaptor2(aBezier);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Surfaces are separated by ~7 units (10 - 3)
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 4.0);
  EXPECT_LT(aMinDist, 10.0);
}

//==================================================================================================
// Offset Surface Tests
//==================================================================================================

TEST_F(ExtremaSS_GenericPairTest, OffsetSphere_VsPlane)
{
  // Offset surface from a sphere
  // This exercises the GenericPair fallback for offset surfaces
  gp_Sphere                     aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);
  Handle(Geom_SphericalSurface) aBaseSphere = new Geom_SphericalSurface(aSphere);

  // Offset creates a larger or smaller sphere depending on sign
  // The actual geometry behavior may vary
  Handle(Geom_OffsetSurface) anOffsetSurf = new Geom_OffsetSurface(aBaseSphere, 1.0);

  // Flat Bezier plane at Z=0 (centered under the sphere)
  Handle(Geom_BezierSurface) aPlane = createBezierPlane(gp_Pnt(-5, -5, 0), 10, 10, gp_Dir(0, 0, 1));

  GeomAdaptor_Surface anAdaptor1(anOffsetSurf);
  GeomAdaptor_Surface anAdaptor2(aPlane);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Just verify we get a reasonable positive distance
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 5.0);
  EXPECT_LT(aMinDist, 15.0);
}

TEST_F(ExtremaSS_GenericPairTest, TwoOffsetSurfaces)
{
  // Two offset surfaces derived from different spheres
  gp_Sphere aSphere1(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 2.0);
  gp_Sphere aSphere2(gp_Ax3(gp_Pnt(10, 0, 0), gp_Dir(0, 0, 1)), 2.0);

  Handle(Geom_SphericalSurface) aBaseSphere1 = new Geom_SphericalSurface(aSphere1);
  Handle(Geom_SphericalSurface) aBaseSphere2 = new Geom_SphericalSurface(aSphere2);

  // Offset both by 1 -> spheres with radius 3
  Handle(Geom_OffsetSurface) anOffset1 = new Geom_OffsetSurface(aBaseSphere1, 1.0);
  Handle(Geom_OffsetSurface) anOffset2 = new Geom_OffsetSurface(aBaseSphere2, 1.0);

  GeomAdaptor_Surface anAdaptor1(anOffset1);
  GeomAdaptor_Surface anAdaptor2(anOffset2);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Two spheres of radius 3, centers 10 apart
  // Minimum distance = 10 - 3 - 3 = 4
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_NEAR(aMinDist, 4.0, 0.5);
}

TEST_F(ExtremaSS_GenericPairTest, BSpline_VsOffsetSurface)
{
  // BSpline wavy surface vs offset sphere
  Handle(Geom_BSplineSurface) aBSpline = createBSplineWavySurface(gp_Pnt(0, 0, 0), 8.0, 8.0, 0.5);

  gp_Sphere                     aSphere(gp_Ax3(gp_Pnt(0, 0, 10), gp_Dir(0, 0, 1)), 3.0);
  Handle(Geom_SphericalSurface) aBaseSphere = new Geom_SphericalSurface(aSphere);
  Handle(Geom_OffsetSurface)    anOffset    = new Geom_OffsetSurface(aBaseSphere, 0.5);

  GeomAdaptor_Surface anAdaptor1(aBSpline);
  GeomAdaptor_Surface anAdaptor2(anOffset);

  ExtremaSS_GenericPair    anEval(anAdaptor1, anAdaptor2);
  const ExtremaSS::Result& aResult = anEval.Perform(THE_TOL);

  ASSERT_EQ(aResult.Status, ExtremaSS::Status::OK);
  ASSERT_GE(aResult.NbExt(), 1);

  // Offset sphere has radius 3.5, center at z=10
  // BSpline wavy surface centered at z=0 with amplitude 0.5
  // Minimum distance ~10 - 3.5 - 0.5 = 6
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 4.0);
  EXPECT_LT(aMinDist, 8.0);
}
