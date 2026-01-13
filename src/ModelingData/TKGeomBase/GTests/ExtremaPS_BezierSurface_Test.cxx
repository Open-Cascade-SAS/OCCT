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
#include <ExtremaPS_BezierSurface.hxx>
#include <ExtremaPS_Surface.hxx>

#include <Geom_BezierSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE = 1.0e-6;

//! Create a flat Bezier surface (essentially a plane)
occ::handle<Geom_BezierSurface> MakeFlatBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 2, 1, 2);
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(2, 1, gp_Pnt(10, 0, 0));
  aPoles.SetValue(1, 2, gp_Pnt(0, 10, 0));
  aPoles.SetValue(2, 2, gp_Pnt(10, 10, 0));
  return new Geom_BezierSurface(aPoles);
}

//! Create a curved Bezier surface (saddle-like)
occ::handle<Geom_BezierSurface> MakeSaddleBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  // Corners
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(3, 1, gp_Pnt(10, 0, 0));
  aPoles.SetValue(1, 3, gp_Pnt(0, 10, 0));
  aPoles.SetValue(3, 3, gp_Pnt(10, 10, 0));
  // Edges
  aPoles.SetValue(2, 1, gp_Pnt(5, 0, -2));
  aPoles.SetValue(2, 3, gp_Pnt(5, 10, -2));
  aPoles.SetValue(1, 2, gp_Pnt(0, 5, 2));
  aPoles.SetValue(3, 2, gp_Pnt(10, 5, 2));
  // Center - raised to create saddle
  aPoles.SetValue(2, 2, gp_Pnt(5, 5, 3));
  return new Geom_BezierSurface(aPoles);
}

//! Create a dome Bezier surface
occ::handle<Geom_BezierSurface> MakeDomeBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  // Corners - at Z=0
  aPoles.SetValue(1, 1, gp_Pnt(0, 0, 0));
  aPoles.SetValue(3, 1, gp_Pnt(10, 0, 0));
  aPoles.SetValue(1, 3, gp_Pnt(0, 10, 0));
  aPoles.SetValue(3, 3, gp_Pnt(10, 10, 0));
  // Edges - slightly raised
  aPoles.SetValue(2, 1, gp_Pnt(5, 0, 2));
  aPoles.SetValue(2, 3, gp_Pnt(5, 10, 2));
  aPoles.SetValue(1, 2, gp_Pnt(0, 5, 2));
  aPoles.SetValue(3, 2, gp_Pnt(10, 5, 2));
  // Center - highest point
  aPoles.SetValue(2, 2, gp_Pnt(5, 5, 5));
  return new Geom_BezierSurface(aPoles);
}

//! Create a high-degree Bezier surface (4x4)
occ::handle<Geom_BezierSurface> MakeHighDegreeBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double aX = (i - 1) * 10.0 / 3.0;
      double aY = (j - 1) * 10.0 / 3.0;
      double aZ = std::sin(aX * 0.3) * std::cos(aY * 0.3) * 2.0;
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }
  return new Geom_BezierSurface(aPoles);
}

//! Create a wavy Bezier surface
occ::handle<Geom_BezierSurface> MakeWavyBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double aX = (i - 1) * 2.5;
      double aY = (j - 1) * 2.5;
      double aZ = std::sin(aX) * std::sin(aY);
      aPoles.SetValue(i, j, gp_Pnt(aX, aY, aZ));
    }
  }
  return new Geom_BezierSurface(aPoles);
}
} // namespace

//==================================================================================================
// Test fixture for ExtremaPS_BezierSurface tests
//==================================================================================================
class ExtremaPS_BezierSurfaceTest : public testing::Test
{
protected:
  void SetUp() override { myFlatSurface = MakeFlatBezier(); }

  occ::handle<Geom_BezierSurface> myFlatSurface;
};

//==================================================================================================
// Basic Projection Tests on Flat Surface
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, FlatSurface_PointAbove)
{
  gp_Pnt                   aP(5.0, 5.0, 10.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 10.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, FlatSurface_PointOnSurface)
{
  gp_Pnt                   aP(5.0, 5.0, 0.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, FlatSurface_PointBelow)
{
  gp_Pnt                   aP(5.0, 5.0, -5.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 5.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, FlatSurface_PointAtCorner)
{
  gp_Pnt                   aP(0.0, 0.0, 3.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 3.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, FlatSurface_PointOutsideDomain)
{
  // Point projects outside the [0,10]x[0,10] domain
  gp_Pnt                   aP(-5.0, 5.0, 0.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Nearest point should be on the boundary
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 5.0, THE_TOLERANCE);
}

//==================================================================================================
// Saddle Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, SaddleSurface_PointAboveCenter)
{
  occ::handle<Geom_BezierSurface> aSaddle = MakeSaddleBezier();
  gp_Pnt                          aP(5.0, 5.0, 10.0);
  ExtremaPS_BezierSurface         anEval(aSaddle, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum distance should be reasonable for point 10 units above a saddle
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_GT(std::sqrt(aMinDist), 5.0);  // At least 5 units
  EXPECT_LT(std::sqrt(aMinDist), 12.0); // At most 12 units
}

TEST_F(ExtremaPS_BezierSurfaceTest, SaddleSurface_PointBelowCenter)
{
  occ::handle<Geom_BezierSurface> aSaddle = MakeSaddleBezier();
  gp_Pnt                          aP(5.0, 5.0, -5.0);
  ExtremaPS_BezierSurface         anEval(aSaddle, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BezierSurfaceTest, SaddleSurface_MultipleExtrema)
{
  occ::handle<Geom_BezierSurface> aSaddle = MakeSaddleBezier();
  gp_Pnt                          aP(5.0, 5.0, 0.0);
  ExtremaPS_BezierSurface         anEval(aSaddle, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  // Saddle may have multiple extrema
  EXPECT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// Dome Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, DomeSurface_PointAbovePeak)
{
  occ::handle<Geom_BezierSurface> aDome = MakeDomeBezier();
  gp_Pnt                          aP(5.0, 5.0, 15.0);
  ExtremaPS_BezierSurface         anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum should be at the dome peak
  int                              aMinIdx = aResult.MinIndex();
  const ExtremaPS::ExtremumResult& aMinExt = aResult.Extrema.Value(aMinIdx);
  EXPECT_NEAR(aMinExt.U, 0.5, 0.1);
  EXPECT_NEAR(aMinExt.V, 0.5, 0.1);
}

TEST_F(ExtremaPS_BezierSurfaceTest, DomeSurface_PointAtCorner)
{
  occ::handle<Geom_BezierSurface> aDome = MakeDomeBezier();
  gp_Pnt                          aP(0.0, 0.0, 10.0);
  ExtremaPS_BezierSurface         anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Distance from corner to point at (0, 0, 10) - corner is at (0, 0, 0)
  // The actual surface passes through the corner, so distance should be ~10
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_GT(std::sqrt(aMinDist), 8.0);  // At least 8 units
  EXPECT_LT(std::sqrt(aMinDist), 11.0); // At most 11 units
}

TEST_F(ExtremaPS_BezierSurfaceTest, DomeSurface_PointUnderDome)
{
  occ::handle<Geom_BezierSurface> aDome = MakeDomeBezier();
  gp_Pnt                          aP(5.0, 5.0, 0.0);
  ExtremaPS_BezierSurface         anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// High-Degree Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, HighDegree_BasicProjection)
{
  occ::handle<Geom_BezierSurface> aHighDeg = MakeHighDegreeBezier();
  gp_Pnt                          aP(5.0, 5.0, 10.0);
  ExtremaPS_BezierSurface         anEval(aHighDeg, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BezierSurfaceTest, HighDegree_MultiplePoints)
{
  occ::handle<Geom_BezierSurface> aHighDeg = MakeHighDegreeBezier();
  ExtremaPS_BezierSurface         anEval(aHighDeg, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));

  // Test multiple points
  std::vector<gp_Pnt> aPoints = {gp_Pnt(2.0, 2.0, 5.0),
                                 gp_Pnt(7.0, 3.0, 3.0),
                                 gp_Pnt(5.0, 8.0, -2.0)};

  for (const auto& aP : aPoints)
  {
    const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);
    EXPECT_EQ(aResult.Status, ExtremaPS::Status::OK);
    EXPECT_GE(aResult.Extrema.Length(), 1);
  }
}

//==================================================================================================
// Wavy Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, WavySurface_PointAbovePeak)
{
  occ::handle<Geom_BezierSurface> aWavy = MakeWavyBezier();
  gp_Pnt                          aP(M_PI / 2.0, M_PI / 2.0, 5.0);
  ExtremaPS_BezierSurface         anEval(aWavy, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

TEST_F(ExtremaPS_BezierSurfaceTest, WavySurface_PointInValley)
{
  occ::handle<Geom_BezierSurface> aWavy = MakeWavyBezier();
  gp_Pnt                          aP(0.0, 0.0, -5.0);
  ExtremaPS_BezierSurface         anEval(aWavy, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// Partial Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, PartialDomain_HalfU)
{
  gp_Pnt                   aP(2.5, 5.0, 5.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 0.5, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // U should be in [0, 0.5]
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_LE(aResult.Extrema.Value(i).U, 0.5 + THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_BezierSurfaceTest, PartialDomain_HalfV)
{
  gp_Pnt                   aP(5.0, 2.5, 5.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 0.5));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // V should be in [0, 0.5]
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_LE(aResult.Extrema.Value(i).V, 0.5 + THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_BezierSurfaceTest, PartialDomain_SmallPatch)
{
  gp_Pnt                   aP(5.0, 5.0, 5.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.4, 0.6, 0.4, 0.6));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// SearchMode Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, SearchMode_MinOnly)
{
  gp_Pnt                   aP(5.0, 5.0, 10.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Min);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    EXPECT_TRUE(aResult.Extrema.Value(i).IsMinimum);
  }
}

TEST_F(ExtremaPS_BezierSurfaceTest, SearchMode_MaxOnly)
{
  gp_Pnt                   aP(5.0, 5.0, 10.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::Max);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  // May not find maxima on flat surface - boundary extrema
}

TEST_F(ExtremaPS_BezierSurfaceTest, SearchMode_DomeMinMax)
{
  occ::handle<Geom_BezierSurface> aDome = MakeDomeBezier();
  gp_Pnt                          aP(5.0, 5.0, 10.0);
  ExtremaPS_BezierSurface         anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult =
    anEval.PerformWithBoundary(aP, THE_TOLERANCE, ExtremaPS::SearchMode::MinMax);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.Extrema.Length(), 1);
}

//==================================================================================================
// Aggregator Integration Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, Aggregator_FlatSurface)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  ExtremaPS_Surface   anExtPS(anAdaptor);

  gp_Pnt                   aP(5.0, 5.0, 7.0);
  const ExtremaPS::Result& aResult = anExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 7.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, Aggregator_DomeSurface)
{
  occ::handle<Geom_BezierSurface> aDome = MakeDomeBezier();
  GeomAdaptor_Surface             anAdaptor(aDome);
  ExtremaPS_Surface               anExtPS(anAdaptor);

  gp_Pnt                   aP(5.0, 5.0, 12.0);
  const ExtremaPS::Result& aResult = anExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Minimum distance from point at Z=12 above dome center
  // Due to Bezier interpolation, the actual dome peak may not reach Z=5
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_GT(std::sqrt(aMinDist), 5.0);  // At least 5 units
  EXPECT_LT(std::sqrt(aMinDist), 12.0); // At most 12 units (to corners)
}

TEST_F(ExtremaPS_BezierSurfaceTest, Aggregator_WithSearchMode)
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

TEST_F(ExtremaPS_BezierSurfaceTest, EdgeCase_VeryClosePoint)
{
  gp_Pnt                   aP(5.0, 5.0, 1.0e-8);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(aMinDist, 0.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, EdgeCase_VeryFarPoint)
{
  gp_Pnt                   aP(5.0, 5.0, 1000.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 1000.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, EdgeCase_PointOnEdge)
{
  gp_Pnt                   aP(5.0, 0.0, 3.0); // On bottom edge
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), 3.0, THE_TOLERANCE);
}

TEST_F(ExtremaPS_BezierSurfaceTest, EdgeCase_DiagonalPoint)
{
  gp_Pnt                   aP(15.0, 15.0, 0.0); // Far diagonal
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Should project to corner (10,10,0)
  double aMinDist = aResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aMinDist), std::sqrt(50.0), THE_TOLERANCE);
}

//==================================================================================================
// Result Verification Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceTest, Verify_PointOnSurface)
{
  occ::handle<Geom_BezierSurface> aDome = MakeDomeBezier();
  gp_Pnt                          aP(3.0, 7.0, 8.0);
  ExtremaPS_BezierSurface         anEval(aDome, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result&        aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  // Verify each extremum point is on the surface
  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt   = aResult.Extrema.Value(i);
    gp_Pnt                           aSurfPt = aDome->Value(anExt.U, anExt.V);
    EXPECT_NEAR(anExt.Point.Distance(aSurfPt), 0.0, THE_TOLERANCE * 10);
  }
}

TEST_F(ExtremaPS_BezierSurfaceTest, Verify_SquareDistanceConsistent)
{
  gp_Pnt                   aP(7.0, 3.0, 4.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
  const ExtremaPS::Result& aResult = anEval.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  ASSERT_GE(aResult.Extrema.Length(), 1);

  for (int i = 0; i < aResult.Extrema.Length(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt         = aResult.Extrema.Value(i);
    double                           aActualSqDist = aP.SquareDistance(anExt.Point);
    EXPECT_NEAR(anExt.SquareDistance, aActualSqDist, THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_BezierSurfaceTest, Verify_ParametersInRange)
{
  gp_Pnt                   aP(5.0, 5.0, 5.0);
  ExtremaPS_BezierSurface  anEval(myFlatSurface, ExtremaPS::Domain2D(0.2, 0.8, 0.3, 0.7));
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
