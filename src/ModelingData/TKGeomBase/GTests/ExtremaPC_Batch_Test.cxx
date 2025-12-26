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

#include <ExtremaPC_Curve.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <GeomAPI_PointsToBSpline.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>
#include <TColgp_Array1OfPnt.hxx>

#include <gtest/gtest.h>
#include <cmath>
#include <random>

namespace
{
//! Helper to create a simple BSpline curve for testing.
Handle(Geom_BSplineCurve) createTestBSpline()
{
  TColgp_Array1OfPnt aPoles(1, 5);
  aPoles.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(1.0, 2.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(2.0, 0.5, 0.0));
  aPoles.SetValue(4, gp_Pnt(3.0, 1.5, 0.0));
  aPoles.SetValue(5, gp_Pnt(4.0, 0.0, 0.0));

  GeomAPI_PointsToBSpline aBuilder(aPoles);
  return aBuilder.Curve();
}

//! Generate random query points around a box.
NCollection_Array1<gp_Pnt> generateRandomPoints(int theCount, double theMin, double theMax, int theSeed)
{
  NCollection_Array1<gp_Pnt>          aPoints(1, theCount);
  std::mt19937                        aGen(theSeed);
  std::uniform_real_distribution<double> aDist(theMin, theMax);

  for (int i = 1; i <= theCount; ++i)
  {
    aPoints.SetValue(i, gp_Pnt(aDist(aGen), aDist(aGen), aDist(aGen)));
  }
  return aPoints;
}

//! Generate points along a line (simulating uniform distribution from another curve).
NCollection_Array1<gp_Pnt> generateLinePoints(const gp_Pnt& theStart, const gp_Pnt& theEnd, int theCount)
{
  NCollection_Array1<gp_Pnt> aPoints(1, theCount);

  for (int i = 1; i <= theCount; ++i)
  {
    double aT = static_cast<double>(i - 1) / static_cast<double>(theCount - 1);
    aPoints.SetValue(i, theStart.Translated(gp_Vec(theStart, theEnd) * aT));
  }
  return aPoints;
}

} // namespace

//==================================================================================================
// Basic Batch Test - Verify batch matches single-point results
//==================================================================================================

TEST(ExtremaPC_BatchTest, Line_BatchMatchesSingle)
{
  gp_Lin aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aGeomLine = new Geom_Line(aLine);

  ExtremaPC_Curve anExtPC(aGeomLine, 0.0, 10.0);

  NCollection_Array1<gp_Pnt> aPoints = generateRandomPoints(50, -5.0, 15.0, 42);
  const double               aTol    = 1.0e-9;

  // Perform batch
  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  // Verify each result matches single-point query
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    const ExtremaPC::Result& aSingleResult = anExtPC.Perform(aPoints.Value(i), aTol);
    const ExtremaPC::Result& aBatchItem    = aBatchResult[i];

    EXPECT_EQ(aSingleResult.IsDone(), aBatchItem.IsDone()) << "Mismatch at index " << i;
    EXPECT_EQ(aSingleResult.NbExt(), aBatchItem.NbExt()) << "Mismatch at index " << i;

    if (aSingleResult.NbExt() > 0 && aBatchItem.NbExt() > 0)
    {
      EXPECT_NEAR(aSingleResult[0].SquareDistance, aBatchItem[0].SquareDistance, 1e-10)
        << "Distance mismatch at index " << i;
      EXPECT_NEAR(aSingleResult[0].Parameter, aBatchItem[0].Parameter, 1e-10)
        << "Parameter mismatch at index " << i;
    }
  }
}

TEST(ExtremaPC_BatchTest, Circle_BatchMatchesSingle)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(aCirc);

  ExtremaPC_Curve anExtPC(aGeomCircle);

  NCollection_Array1<gp_Pnt> aPoints = generateRandomPoints(30, -10.0, 10.0, 123);
  const double               aTol    = 1.0e-9;

  // Perform batch
  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  // Verify each result matches single-point query
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    const ExtremaPC::Result& aSingleResult = anExtPC.Perform(aPoints.Value(i), aTol);
    const ExtremaPC::Result& aBatchItem    = aBatchResult[i];

    EXPECT_EQ(aSingleResult.IsDone(), aBatchItem.IsDone()) << "Mismatch at index " << i;
    EXPECT_EQ(aSingleResult.NbExt(), aBatchItem.NbExt()) << "Mismatch at index " << i;

    if (aSingleResult.NbExt() > 0 && aBatchItem.NbExt() > 0)
    {
      // Compare minimum distance
      EXPECT_NEAR(aSingleResult.MinSquareDistance(), aBatchItem.MinSquareDistance(), 1e-10)
        << "Min distance mismatch at index " << i;
    }
  }
}

TEST(ExtremaPC_BatchTest, Ellipse_BatchMatchesSingle)
{
  gp_Elips anElips(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0, 3.0);
  Handle(Geom_Ellipse) aGeomEllipse = new Geom_Ellipse(anElips);

  ExtremaPC_Curve anExtPC(aGeomEllipse);

  NCollection_Array1<gp_Pnt> aPoints = generateRandomPoints(20, -8.0, 8.0, 456);
  const double               aTol    = 1.0e-9;

  // Perform batch
  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  // Verify each result matches single-point query
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    const ExtremaPC::Result& aSingleResult = anExtPC.Perform(aPoints.Value(i), aTol);
    const ExtremaPC::Result& aBatchItem    = aBatchResult[i];

    EXPECT_EQ(aSingleResult.IsDone(), aBatchItem.IsDone()) << "Mismatch at index " << i;

    if (aSingleResult.NbExt() > 0 && aBatchItem.NbExt() > 0)
    {
      EXPECT_NEAR(aSingleResult.MinSquareDistance(), aBatchItem.MinSquareDistance(), 1e-10)
        << "Min distance mismatch at index " << i;
    }
  }
}

TEST(ExtremaPC_BatchTest, BSpline_BatchMatchesSingle)
{
  Handle(Geom_BSplineCurve) aBSpline = createTestBSpline();

  ExtremaPC_Curve anExtPC(aBSpline);

  NCollection_Array1<gp_Pnt> aPoints = generateRandomPoints(30, -1.0, 5.0, 789);
  const double               aTol    = 1.0e-9;

  // Perform batch
  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  // Verify each result matches single-point query
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    const ExtremaPC::Result& aSingleResult = anExtPC.Perform(aPoints.Value(i), aTol);
    const ExtremaPC::Result& aBatchItem    = aBatchResult[i];

    EXPECT_EQ(aSingleResult.IsDone(), aBatchItem.IsDone()) << "Mismatch at index " << i;

    if (aSingleResult.NbExt() > 0 && aBatchItem.NbExt() > 0)
    {
      // Compare minimum distance - allow slightly larger tolerance due to KD-Tree approximation
      EXPECT_NEAR(aSingleResult.MinSquareDistance(), aBatchItem.MinSquareDistance(), 1e-8)
        << "Min distance mismatch at index " << i;
    }
  }
}

//==================================================================================================
// Edge Cases
//==================================================================================================

TEST(ExtremaPC_BatchTest, EmptyPointsArray)
{
  gp_Lin aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aGeomLine = new Geom_Line(aLine);

  ExtremaPC_Curve anExtPC(aGeomLine, 0.0, 10.0);

  NCollection_Array1<gp_Pnt> aEmptyPoints;
  const double               aTol = 1.0e-9;

  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aEmptyPoints, aTol);

  EXPECT_TRUE(aBatchResult.IsEmpty());
}

TEST(ExtremaPC_BatchTest, SinglePoint)
{
  gp_Lin aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  Handle(Geom_Line) aGeomLine = new Geom_Line(aLine);

  ExtremaPC_Curve anExtPC(aGeomLine, 0.0, 10.0);

  NCollection_Array1<gp_Pnt> aPoints(1, 1);
  aPoints.SetValue(1, gp_Pnt(5.0, 3.0, 0.0));

  const double aTol = 1.0e-9;

  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  EXPECT_EQ(1, aBatchResult.Size());
  EXPECT_TRUE(aBatchResult[1].IsDone());
  EXPECT_EQ(1, aBatchResult[1].NbExt());
  EXPECT_NEAR(9.0, aBatchResult[1][0].SquareDistance, 1e-10); // 3^2 = 9
}

//==================================================================================================
// Uniformly Distributed Points (simulating samples from another curve)
//==================================================================================================

TEST(ExtremaPC_BatchTest, UniformLinePoints_OnBSpline)
{
  Handle(Geom_BSplineCurve) aBSpline = createTestBSpline();
  ExtremaPC_Curve           anExtPC(aBSpline);

  // Generate points along a parallel line
  NCollection_Array1<gp_Pnt> aPoints =
    generateLinePoints(gp_Pnt(0.0, 5.0, 0.0), gp_Pnt(4.0, 5.0, 0.0), 20);

  const double aTol = 1.0e-9;

  // Perform batch
  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  // All results should be done
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    EXPECT_TRUE(aBatchResult[i].IsDone()) << "Query " << i << " not done";
    EXPECT_GT(aBatchResult[i].NbExt(), 0) << "Query " << i << " has no extrema";
  }
}

//==================================================================================================
// GlobalMinSquareDistance Test
//==================================================================================================

TEST(ExtremaPC_BatchTest, GlobalMinSquareDistance)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(aCirc);

  ExtremaPC_Curve anExtPC(aGeomCircle);

  NCollection_Array1<gp_Pnt> aPoints(1, 5);
  aPoints.SetValue(1, gp_Pnt(10.0, 0.0, 0.0)); // Distance to circle = 5
  aPoints.SetValue(2, gp_Pnt(6.0, 0.0, 0.0));  // Distance to circle = 1
  aPoints.SetValue(3, gp_Pnt(5.0, 0.0, 0.0));  // Distance to circle = 0 (on circle)
  aPoints.SetValue(4, gp_Pnt(0.0, 7.0, 0.0));  // Distance to circle = 2
  aPoints.SetValue(5, gp_Pnt(0.0, 0.0, 3.0));  // Distance to circle = sqrt(25 + 9) - on axis

  const double aTol = 1.0e-9;

  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  int    aMinPointIdx;
  int    aMinExtIdx;
  double aGlobalMinSqDist = aBatchResult.GlobalMinSquareDistance(aMinPointIdx, aMinExtIdx);

  // Point 3 (index 3) is on the circle, so distance should be 0
  EXPECT_EQ(3, aMinPointIdx);
  EXPECT_NEAR(0.0, aGlobalMinSqDist, 1e-10);
}

//==================================================================================================
// Search Mode Tests
//==================================================================================================

TEST(ExtremaPC_BatchTest, SearchMode_MinOnly)
{
  gp_Circ aCirc(gp_Ax2(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0)), 5.0);
  Handle(Geom_Circle) aGeomCircle = new Geom_Circle(aCirc);

  ExtremaPC_Curve anExtPC(aGeomCircle);

  NCollection_Array1<gp_Pnt> aPoints(1, 3);
  aPoints.SetValue(1, gp_Pnt(0.0, 0.0, 0.0));  // Center - min and max are equal
  aPoints.SetValue(2, gp_Pnt(10.0, 0.0, 0.0)); // Outside - has min and max
  aPoints.SetValue(3, gp_Pnt(3.0, 0.0, 0.0));  // Inside - has min and max

  const double aTol = 1.0e-9;

  // Test Min mode
  ExtremaPC::BatchResult aMinResult = anExtPC.PerformBatch(aPoints, aTol, ExtremaPC::SearchMode::Min);

  // All results should have minimum extrema only
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    if (aMinResult[i].NbExt() > 0)
    {
      for (int j = 0; j < aMinResult[i].NbExt(); ++j)
      {
        EXPECT_TRUE(aMinResult[i][j].IsMinimum) << "Expected minimum at index " << i << ", extremum " << j;
      }
    }
  }
}

//==================================================================================================
// Large Batch Test
//==================================================================================================

TEST(ExtremaPC_BatchTest, LargeBatch_Performance)
{
  Handle(Geom_BSplineCurve) aBSpline = createTestBSpline();
  ExtremaPC_Curve           anExtPC(aBSpline);

  // Generate 1000 random query points
  NCollection_Array1<gp_Pnt> aPoints = generateRandomPoints(1000, -2.0, 6.0, 12345);

  const double aTol = 1.0e-9;

  // This should complete without timeout and produce valid results
  ExtremaPC::BatchResult aBatchResult = anExtPC.PerformBatch(aPoints, aTol);

  EXPECT_EQ(1000, aBatchResult.Size());

  // Count successful queries
  int aDoneCount = 0;
  for (int i = aPoints.Lower(); i <= aPoints.Upper(); ++i)
  {
    if (aBatchResult[i].IsDone())
    {
      ++aDoneCount;
    }
  }

  // All queries should succeed
  EXPECT_EQ(1000, aDoneCount);
}
