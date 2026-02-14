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

#include <ExtremaPS.hxx>
#include <ExtremaPS_BSplineSurface.hxx>
#include <ExtremaPS_Surface.hxx>

#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <GeomAPI_PointsToBSplineSurface.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfReal.hxx>
#include <TColStd_Array1OfInteger.hxx>

#include <cmath>
#include <vector>

namespace
{
const double THE_TOLERANCE = 1.0e-6;

//! Create a flat BSpline surface (plane-like)
Handle(Geom_BSplineSurface) MakeFlatBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double aX = (i - 1) * 10.0 / 3.0;
      double aY = (j - 1) * 10.0 / 3.0;
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, 0.0));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 2), aVKnots(1, 2);
  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  aUMults(1) = 4;
  aUMults(2) = 4;
  aVMults(1) = 4;
  aVMults(2) = 4;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
}

//! Create a curved BSpline surface (dome-like)
//! For 5 poles with degree 2: sum of multiplicities = 5 + 2 + 1 = 8
Handle(Geom_BSplineSurface) MakeDomeBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double aX  = (i - 1) * 2.5;
      double aY  = (j - 1) * 2.5;
      double aR  = std::sqrt((aX - 5.0) * (aX - 5.0) + (aY - 5.0) * (aY - 5.0));
      double aZ  = std::max(0.0, 5.0 - aR * 0.5);
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 3), aVKnots(1, 3);
  TColStd_Array1OfInteger aUMults(1, 3), aVMults(1, 3);
  aUKnots(1) = 0.0;
  aUKnots(2) = 0.5;
  aUKnots(3) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.5;
  aVKnots(3) = 1.0;
  // For 5 poles, deg 2: need 5+2+1 = 8 total multiplicity
  aUMults(1) = 3;  // degree + 1 at start
  aUMults(2) = 2;  // 8 - 3 - 3 = 2
  aUMults(3) = 3;  // degree + 1 at end
  aVMults(1) = 3;
  aVMults(2) = 2;
  aVMults(3) = 3;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

//! Create a wavy BSpline surface
//! For 7 poles with degree 3: sum of multiplicities = 7 + 3 + 1 = 11
Handle(Geom_BSplineSurface) MakeWavyBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 7, 1, 7);
  for (int i = 1; i <= 7; ++i)
  {
    for (int j = 1; j <= 7; ++j)
    {
      double aX = (i - 1) * 10.0 / 6.0;
      double aY = (j - 1) * 10.0 / 6.0;
      double aZ = std::sin(aX * 0.6) * std::cos(aY * 0.6) * 2.0;
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 4), aVKnots(1, 4);
  TColStd_Array1OfInteger aUMults(1, 4), aVMults(1, 4);
  aUKnots(1) = 0.0;
  aUKnots(2) = 0.33;
  aUKnots(3) = 0.67;
  aUKnots(4) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.33;
  aVKnots(3) = 0.67;
  aVKnots(4) = 1.0;
  // For 7 poles, deg 3: need 7+3+1 = 11 total multiplicity
  aUMults(1) = 4;  // degree + 1 at start
  aUMults(2) = 2;  // 11 - 4 - 4 - 1 = 2
  aUMults(3) = 1;
  aUMults(4) = 4;  // degree + 1 at end
  aVMults(1) = 4;
  aVMults(2) = 2;
  aVMults(3) = 1;
  aVMults(4) = 4;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
}

//! Create a saddle-like BSpline surface
//! For 5 poles with degree 2: sum of multiplicities = 5 + 2 + 1 = 8
Handle(Geom_BSplineSurface) MakeSaddleBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double aX = (i - 1) * 2.5;
      double aY = (j - 1) * 2.5;
      double aZ = ((aX - 5.0) * (aX - 5.0) - (aY - 5.0) * (aY - 5.0)) * 0.1;
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 3), aVKnots(1, 3);
  TColStd_Array1OfInteger aUMults(1, 3), aVMults(1, 3);
  aUKnots(1) = 0.0;
  aUKnots(2) = 0.5;
  aUKnots(3) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.5;
  aVKnots(3) = 1.0;
  // For 5 poles, deg 2: need 5+2+1 = 8 total multiplicity
  aUMults(1) = 3;  // degree + 1 at start
  aUMults(2) = 2;  // 8 - 3 - 3 = 2
  aUMults(3) = 3;  // degree + 1 at end
  aVMults(1) = 3;
  aVMults(2) = 2;
  aVMults(3) = 3;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

//! Create a high-degree BSpline surface
Handle(Geom_BSplineSurface) MakeHighDegreeBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 6, 1, 6);
  for (int i = 1; i <= 6; ++i)
  {
    for (int j = 1; j <= 6; ++j)
    {
      double aX = (i - 1) * 2.0;
      double aY = (j - 1) * 2.0;
      double aZ = std::sin(aX * 0.5) * std::sin(aY * 0.5) * 3.0;
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 2), aVKnots(1, 2);
  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);
  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;
  aUMults(1) = 6;
  aUMults(2) = 6;
  aVMults(1) = 6;
  aVMults(2) = 6;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 5, 5);
}
} // namespace

//==================================================================================================
// Test fixture for ExtremaPS_BSplineSurface tests
//==================================================================================================
class ExtremaPS_BSplineSurfaceTest : public testing::Test
{
protected:
  void SetUp() override { myFlatSurface = MakeFlatBSpline(); }

  Handle(Geom_BSplineSurface) myFlatSurface;
};

//==================================================================================================
// Basic Projection Tests on Flat Surface
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, FlatSurface_PointAbove)
{
  gp_Pnt                    aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 10.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, FlatSurface_PointOnSurface)
{
  gp_Pnt                    aP(5.0, 5.0, 0.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, FlatSurface_PointBelow)
{
  gp_Pnt                    aP(5.0, 5.0, -5.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 5.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, FlatSurface_PointAtCorner)
{
  gp_Pnt                    aP(0.0, 0.0, 3.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 3.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, FlatSurface_PointOutsideDomain)
{
  gp_Pnt                    aP(-5.0, 5.0, 0.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 5.0, THE_TOLERANCE);
}

//==================================================================================================
// Dome Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, DomeSurface_PointAbovePeak)
{
  Handle(Geom_BSplineSurface) aDome = MakeDomeBSpline();
  gp_Pnt                      aP(5.0, 5.0, 15.0);
  ExtremaPS_BSplineSurface   anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum distance to dome from point 15 units above center should be reasonable
  // The dome peak is approximately at Z=5, so minimum distance should be around 10
  double aMinDist = std::sqrt(aResult.MinSquareDistance());
  EXPECT_GT(aMinDist, 5.0);   // At least 5 units away
  EXPECT_LT(aMinDist, 15.0);  // At most 15 units away (from corners)
}

TEST_F(ExtremaPS_BSplineSurfaceTest, DomeSurface_PointAtEdge)
{
  Handle(Geom_BSplineSurface) aDome = MakeDomeBSpline();
  gp_Pnt                      aP(0.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface   anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, DomeSurface_PointUnderDome)
{
  Handle(Geom_BSplineSurface) aDome = MakeDomeBSpline();
  gp_Pnt                      aP(5.0, 5.0, -5.0);
  ExtremaPS_BSplineSurface   anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// Wavy Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, WavySurface_PointAbovePeak)
{
  Handle(Geom_BSplineSurface) aWavy = MakeWavyBSpline();
  gp_Pnt                      aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface   anEval(aWavy, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, WavySurface_PointInValley)
{
  Handle(Geom_BSplineSurface) aWavy = MakeWavyBSpline();
  gp_Pnt                      aP(5.0, 0.0, -5.0);
  ExtremaPS_BSplineSurface   anEval(aWavy, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, WavySurface_MultipleExtrema)
{
  Handle(Geom_BSplineSurface) aWavy = MakeWavyBSpline();
  gp_Pnt                      aP(5.0, 5.0, 0.0);
  ExtremaPS_BSplineSurface   anEval(aWavy, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// Saddle Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, SaddleSurface_PointAboveCenter)
{
  Handle(Geom_BSplineSurface) aSaddle = MakeSaddleBSpline();
  gp_Pnt                      aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface   anEval(aSaddle, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, SaddleSurface_PointAtSaddlePoint)
{
  Handle(Geom_BSplineSurface) aSaddle = MakeSaddleBSpline();
  gp_Pnt                      aP(5.0, 5.0, 5.0);
  ExtremaPS_BSplineSurface   anEval(aSaddle, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// High-Degree Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, HighDegree_BasicProjection)
{
  Handle(Geom_BSplineSurface) aHighDeg = MakeHighDegreeBSpline();
  gp_Pnt                      aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface   anEval(aHighDeg, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, HighDegree_MultiplePoints)
{
  Handle(Geom_BSplineSurface) aHighDeg = MakeHighDegreeBSpline();
  ExtremaPS_BSplineSurface   anEval(aHighDeg, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));

  std::vector<gp_Pnt> aPoints = {gp_Pnt(2.0, 2.0, 5.0), gp_Pnt(7.0, 3.0, 3.0), gp_Pnt(5.0, 8.0, -2.0)};

  for (const auto& aP : aPoints)
  {
    const ExtremaPS::Result& aResult =anEval.PerformWithBoundary(aP, THE_TOLERANCE);
    EXPECT_EQ(aResult.Status, ExtremaPS::Status::OK);
    EXPECT_GE(aResult.Extrema.Length(), 1);
  }
}

//==================================================================================================
// Partial Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, PartialDomain_HalfU)
{
  gp_Pnt                    aP(2.5, 5.0, 5.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 0.5, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_LE(aResult.Extrema.Value(i).U, 0.5 + THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_BSplineSurfaceTest, PartialDomain_HalfV)
{
  gp_Pnt                    aP(5.0, 2.5, 5.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 0.5));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_LE(aResult.Extrema.Value(i).V, 0.5 + THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_BSplineSurfaceTest, PartialDomain_SmallPatch)
{
  gp_Pnt                    aP(5.0, 5.0, 5.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.4, 0.6, 0.4, 0.6));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, PartialDomain_SingleKnotSpan)
{
  Handle(Geom_BSplineSurface) aWavy = MakeWavyBSpline();
  gp_Pnt                      aP(3.0, 3.0, 5.0);
  ExtremaPS_BSplineSurface   anEval(aWavy, ExtremaPS::Domain2D(0.0, 0.33, 0.0, 0.33));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, SearchMode_MinOnly)
{
  gp_Pnt                    aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_TRUE(aResult.Extrema.Value(i).IsMinimum);
  }
}

TEST_F(ExtremaPS_BSplineSurfaceTest, SearchMode_MaxOnly)
{
  gp_Pnt                    aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, SearchMode_DomeMinMax)
{
  Handle(Geom_BSplineSurface) aDome = MakeDomeBSpline();
  gp_Pnt                      aP(5.0, 5.0, 10.0);
  ExtremaPS_BSplineSurface   anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// Aggregator Integration Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, Aggregator_FlatSurface)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  ExtremaPS_Surface   anExtPS(anAdaptor);

  gp_Pnt            aP(5.0, 5.0, 7.0);
  const ExtremaPS::Result& aResult =anExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 7.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, Aggregator_DomeSurface)
{
  Handle(Geom_BSplineSurface) aDome = MakeDomeBSpline();
  GeomAdaptor_Surface         anAdaptor(aDome);
  ExtremaPS_Surface           anExtPS(anAdaptor);

  gp_Pnt            aP(5.0, 5.0, 12.0);
  const ExtremaPS::Result& aResult =anExtPS.Perform(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, Aggregator_WithSearchMode)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  ExtremaPS_Surface   anExtPS(anAdaptor);

  gp_Pnt                   aP(5.0, 5.0, 7.0);
  const ExtremaPS::Result& aResult =
    anExtPS.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_TRUE(aResult.Extrema.Value(i).IsMinimum);
  }
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, EdgeCase_VeryClosePoint)
{
  gp_Pnt                    aP(5.0, 5.0, 1.0e-8);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, EdgeCase_VeryFarPoint)
{
  gp_Pnt                    aP(5.0, 5.0, 1000.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 1000.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, EdgeCase_PointOnEdge)
{
  gp_Pnt                    aP(5.0, 0.0, 3.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 3.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, EdgeCase_DiagonalPoint)
{
  gp_Pnt                    aP(15.0, 15.0, 0.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), std::sqrt(50.0), THE_TOLERANCE);
}

//==================================================================================================
// Result Verification Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, Verify_PointOnSurface)
{
  Handle(Geom_BSplineSurface) aDome = MakeDomeBSpline();
  gp_Pnt                      aP(3.0, 7.0, 8.0);
  ExtremaPS_BSplineSurface   anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt   = aResult.Extrema.Value(i);
    gp_Pnt                           aSurfPt = aDome->Value(anExt.U, anExt.V);
    EXPECT_NEAR(anExt.Point.Distance(aSurfPt), 0.0, THE_TOLERANCE * 10);
  }
}

TEST_F(ExtremaPS_BSplineSurfaceTest, Verify_SquareDistanceConsistent)
{
  gp_Pnt                    aP(7.0, 3.0, 4.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt        = aResult.Extrema.Value(i);
    double                           aActualSqDist = aP.SquareDistance(anExt.Point);
    EXPECT_NEAR(anExt.SquareDistance, aActualSqDist, THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_BSplineSurfaceTest, Verify_ParametersInRange)
{
  gp_Pnt                    aP(5.0, 5.0, 5.0);
  ExtremaPS_BSplineSurface anEval(myFlatSurface, ExtremaPS::Domain2D(0.2, 0.8, 0.3, 0.7));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt = aResult.Extrema.Value(i);
    EXPECT_GE(anExt.U, 0.2 - THE_TOLERANCE);
    EXPECT_LE(anExt.U, 0.8 + THE_TOLERANCE);
    EXPECT_GE(anExt.V, 0.3 - THE_TOLERANCE);
    EXPECT_LE(anExt.V, 0.7 + THE_TOLERANCE);
  }
}

//==================================================================================================
// Knot-Specific Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceTest, Knots_PointNearKnot)
{
  Handle(Geom_BSplineSurface) aWavy = MakeWavyBSpline();
  // Point near internal knot at U=0.33
  gp_Pnt                    aP(3.3, 5.0, 5.0);
  ExtremaPS_BSplineSurface anEval(aWavy, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BSplineSurfaceTest, Knots_CrossingKnotSpan)
{
  Handle(Geom_BSplineSurface) aWavy = MakeWavyBSpline();
  // Range that crosses knot at 0.33
  gp_Pnt                    aP(5.0, 5.0, 5.0);
  ExtremaPS_BSplineSurface anEval(aWavy, ExtremaPS::Domain2D(0.2, 0.5, 0.2, 0.5));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
}
