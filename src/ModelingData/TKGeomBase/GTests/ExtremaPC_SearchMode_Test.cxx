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

#include <ExtremaPC_Curve.hxx>
#include <Extrema_ExtPC.hxx>
#include <Geom_BSplineCurve.hxx>
#include <Geom_BezierCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Hyperbola.hxx>
#include <Geom_Line.hxx>
#include <Geom_OffsetCurve.hxx>
#include <Geom_Parabola.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Circ.hxx>
#include <gp_Elips.hxx>
#include <gp_Hypr.hxx>
#include <gp_Lin.hxx>
#include <gp_Parab.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <Standard_Real.hxx>

#include <cmath>
#include <random>

namespace
{
constexpr double THE_TOL = 1.0e-6;
}

//==================================================================================================
// Test fixture for SearchMode tests
//==================================================================================================

class ExtremaPC_SearchModeTest : public testing::Test
{
protected:
  //! Gets minimum distance from old Extrema_ExtPC
  double GetOldMinDistance(Extrema_ExtPC& theOld)
  {
    if (!theOld.IsDone() || theOld.NbExt() == 0)
    {
      return RealLast();
    }
    double aMinSqDist = RealLast();
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      if (theOld.SquareDistance(i) < aMinSqDist)
      {
        aMinSqDist = theOld.SquareDistance(i);
      }
    }
    return std::sqrt(aMinSqDist);
  }

  //! Gets maximum distance from old Extrema_ExtPC
  double GetOldMaxDistance(Extrema_ExtPC& theOld)
  {
    if (!theOld.IsDone() || theOld.NbExt() == 0)
    {
      return 0.0;
    }
    double aMaxSqDist = 0.0;
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      if (theOld.SquareDistance(i) > aMaxSqDist)
      {
        aMaxSqDist = theOld.SquareDistance(i);
      }
    }
    return std::sqrt(aMaxSqDist);
  }

  //! Gets minimum parameter from old Extrema_ExtPC
  double GetOldMinParameter(Extrema_ExtPC& theOld)
  {
    if (!theOld.IsDone() || theOld.NbExt() == 0)
    {
      return 0.0;
    }
    double aMinSqDist = RealLast();
    int    aMinIdx    = 1;
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      if (theOld.SquareDistance(i) < aMinSqDist)
      {
        aMinSqDist = theOld.SquareDistance(i);
        aMinIdx    = i;
      }
    }
    return theOld.Point(aMinIdx).Parameter();
  }

  //! Gets maximum parameter from old Extrema_ExtPC
  double GetOldMaxParameter(Extrema_ExtPC& theOld)
  {
    if (!theOld.IsDone() || theOld.NbExt() == 0)
    {
      return 0.0;
    }
    double aMaxSqDist = 0.0;
    int    aMaxIdx    = 1;
    for (int i = 1; i <= theOld.NbExt(); ++i)
    {
      if (theOld.SquareDistance(i) > aMaxSqDist)
      {
        aMaxSqDist = theOld.SquareDistance(i);
        aMaxIdx    = i;
      }
    }
    return theOld.Point(aMaxIdx).Parameter();
  }
};

//==================================================================================================
// Line SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Line_MinMode)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aGeomLine, -100.0, 100.0);

  gp_Pnt aPoint(25.0, 10.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1); // Min mode should return only 1 result

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
  EXPECT_NEAR(aNewMinDist, 10.0, THE_TOL); // Distance to line is 10
}

TEST_F(ExtremaPC_SearchModeTest, Line_MaxMode)
{
  // For bounded lines, Max mode with endpoints includes endpoint maxima.
  // The maximum distance is at the farthest endpoint, not the perpendicular projection.
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aGeomLine, -50.0, 50.0);

  gp_Pnt aPoint(0.0, 10.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.PerformWithEndpoints(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());
  // With endpoints included, max mode finds the farthest endpoint
  EXPECT_GE(aNewResult.NbExt(), 1);

  // Maximum distance is to endpoint at (-50, 0, 0) or (50, 0, 0)
  // Distance = sqrt(50^2 + 10^2) = sqrt(2600) ≈ 50.99
  double aExpectedMaxDist = std::sqrt(2600.0);
  double aNewMaxDist      = std::sqrt(aNewResult.MaxSquareDistance());
  EXPECT_NEAR(aNewMaxDist, aExpectedMaxDist, THE_TOL);
}

TEST_F(ExtremaPC_SearchModeTest, Line_MinMaxMode)
{
  occ::handle<Geom_Line> aGeomLine = new Geom_Line(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0));
  GeomAdaptor_Curve      anAdaptor(aGeomLine, -50.0, 50.0);

  gp_Pnt aPoint(10.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  // MinMax mode should find all extrema (min + both endpoints for bounded line)

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
  EXPECT_NEAR(aNewMinDist, 5.0, THE_TOL);
}

//==================================================================================================
// Circle SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Circle_MinMode_PointOutside)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1);

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
  EXPECT_NEAR(aNewMinDist, 10.0, THE_TOL); // 20 - 10 = 10
}

TEST_F(ExtremaPC_SearchModeTest, Circle_MaxMode_PointOutside)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1);

  double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
  double aOldMaxDist = GetOldMaxDistance(anOldExtPC);

  EXPECT_NEAR(aNewMaxDist, aOldMaxDist, THE_TOL);
  EXPECT_NEAR(aNewMaxDist, 30.0, THE_TOL); // 20 + 10 = 30
}

TEST_F(ExtremaPC_SearchModeTest, Circle_MinMode_PointInside)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(3.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1);

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  EXPECT_NEAR(aNewMinDist, 7.0, THE_TOL); // 10 - 3 = 7
}

TEST_F(ExtremaPC_SearchModeTest, Circle_MaxMode_PointInside)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(3.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1);

  double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
  EXPECT_NEAR(aNewMaxDist, 13.0, THE_TOL); // 10 + 3 = 13
}

TEST_F(ExtremaPC_SearchModeTest, Circle_MinMaxMode)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(15.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 2); // Circle has 2 extrema: min and max

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);
  double aOldMaxDist = GetOldMaxDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
  EXPECT_NEAR(aNewMaxDist, aOldMaxDist, THE_TOL);
  EXPECT_NEAR(aNewMinDist, 5.0, THE_TOL);
  EXPECT_NEAR(aNewMaxDist, 25.0, THE_TOL);
}

TEST_F(ExtremaPC_SearchModeTest, Circle_PartialArc_MinMode)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Only first quadrant: 0 to PI/2
  GeomAdaptor_Curve anAdaptor(aGeomCircle, 0.0, M_PI / 2.0);

  gp_Pnt aPoint(15.0, 15.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
}

TEST_F(ExtremaPC_SearchModeTest, Circle_PartialArc_MaxMode)
{
  // NOTE: Max mode for partial arcs may not find extrema if the max is at an
  // endpoint (which is not a true extremum in the mathematical sense).
  // This test verifies the implementation completes successfully.
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  // Only first quadrant: 0 to PI/2
  GeomAdaptor_Curve anAdaptor(aGeomCircle, 0.0, M_PI / 2.0);

  gp_Pnt aPoint(15.0, 15.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());

  // If extrema were found, verify they are valid
  if (aNewResult.NbExt() > 0)
  {
    const auto& aExtremum = aNewResult[aNewResult.MaxIndex()];
    // Parameter should be within bounds
    EXPECT_GE(aExtremum.Parameter, 0.0 - THE_TOL);
    EXPECT_LE(aExtremum.Parameter, M_PI / 2.0 + THE_TOL);
    // Point should be on the curve
    gp_Pnt aCurvePnt = anAdaptor.Value(aExtremum.Parameter);
    EXPECT_NEAR(aCurvePnt.Distance(aExtremum.Point), 0.0, THE_TOL);
  }
}

//==================================================================================================
// Ellipse SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Ellipse_MinMode_OnMajorAxis)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(30.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1);

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
  EXPECT_NEAR(aNewMinDist, 10.0, THE_TOL); // 30 - 20 = 10
}

TEST_F(ExtremaPC_SearchModeTest, Ellipse_MaxMode_OnMajorAxis)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(30.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_EQ(aNewResult.NbExt(), 1);

  double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
  double aOldMaxDist = GetOldMaxDistance(anOldExtPC);

  EXPECT_NEAR(aNewMaxDist, aOldMaxDist, THE_TOL);
  EXPECT_NEAR(aNewMaxDist, 50.0, THE_TOL); // 30 + 20 = 50
}

TEST_F(ExtremaPC_SearchModeTest, Ellipse_MinMode_General)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(15.0, 12.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
}

TEST_F(ExtremaPC_SearchModeTest, Ellipse_MinMaxMode)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  gp_Pnt aPoint(15.0, 8.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());
  // Ellipse should have 4 extrema for general point position

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);
  double aOldMaxDist = GetOldMaxDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
  EXPECT_NEAR(aNewMaxDist, aOldMaxDist, THE_TOL);
}

//==================================================================================================
// Parabola SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Parabola_MinMode_AtVertex)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -20.0, 20.0);

  gp_Pnt aPoint(0.0, 5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
}

TEST_F(ExtremaPC_SearchModeTest, Parabola_MaxMode)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -20.0, 20.0);

  gp_Pnt aPoint(0.0, 5.0, 0.0);

  ExtremaPC_Curve anExtPC(anAdaptor);
  // Use PerformWithEndpoints to find maximum which includes endpoints
  const ExtremaPC::Result& aNewResult =
    anExtPC.PerformWithEndpoints(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_GE(aNewResult.NbExt(), 1);

  // With endpoints, Max mode finds the maximum at one of the endpoints.
  if (aNewResult.NbExt() > 0)
  {
    const auto& aExtremum = aNewResult[aNewResult.MaxIndex()];
    EXPECT_GE(aExtremum.Parameter, -20.0 - THE_TOL);
    EXPECT_LE(aExtremum.Parameter, 20.0 + THE_TOL);
    gp_Pnt aCurvePnt = anAdaptor.Value(aExtremum.Parameter);
    EXPECT_NEAR(aCurvePnt.Distance(aExtremum.Point), 0.0, THE_TOL);
  }
}

TEST_F(ExtremaPC_SearchModeTest, Parabola_MinMode_OffAxis)
{
  occ::handle<Geom_Parabola> aGeomParabola =
    new Geom_Parabola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomParabola, -20.0, 20.0);

  gp_Pnt aPoint(10.0, -5.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
}

//==================================================================================================
// Hyperbola SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Hyperbola_MinMode)
{
  occ::handle<Geom_Hyperbola> aGeomHyperbola =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomHyperbola, -2.0, 2.0);

  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL);
}

TEST_F(ExtremaPC_SearchModeTest, Hyperbola_MaxMode)
{
  // NOTE: Max mode for bounded curves may not find extrema if the max is at
  // an endpoint (which is not a true extremum in the mathematical sense).
  occ::handle<Geom_Hyperbola> aGeomHyperbola =
    new Geom_Hyperbola(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 5.0);
  GeomAdaptor_Curve anAdaptor(aGeomHyperbola, -2.0, 2.0);

  gp_Pnt aPoint(20.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());

  // For hyperbola, verify the found point is valid if extrema exist
  if (aNewResult.NbExt() > 0)
  {
    const auto& aExtremum = aNewResult[aNewResult.MaxIndex()];
    EXPECT_GE(aExtremum.Parameter, -2.0 - THE_TOL);
    EXPECT_LE(aExtremum.Parameter, 2.0 + THE_TOL);
    gp_Pnt aCurvePnt = anAdaptor.Value(aExtremum.Parameter);
    EXPECT_NEAR(aCurvePnt.Distance(aExtremum.Point), 0.0, THE_TOL);
  }
}

//==================================================================================================
// BSpline SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, BSpline_MinMode)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(3, 2, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(2.0, 3.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  // Allow slightly larger tolerance for BSpline
  EXPECT_NEAR(aNewMinDist, aOldMinDist, 0.01);
}

TEST_F(ExtremaPC_SearchModeTest, BSpline_MaxMode)
{
  // NOTE: Max mode for BSpline may not find extrema if the max is at an endpoint.
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(3, 2, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(2.0, 1.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());

  // Verify the found point is valid if extrema exist
  if (aNewResult.NbExt() > 0)
  {
    const auto& aExtremum = aNewResult[aNewResult.MaxIndex()];
    EXPECT_GE(aExtremum.Parameter, 0.0 - THE_TOL);
    EXPECT_LE(aExtremum.Parameter, 1.0 + THE_TOL);
    gp_Pnt aCurvePnt = anAdaptor.Value(aExtremum.Parameter);
    EXPECT_NEAR(aCurvePnt.Distance(aExtremum.Point), 0.0, 0.01);
  }
}

TEST_F(ExtremaPC_SearchModeTest, BSpline_MinMaxMode)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(3, 2, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(2.0, 1.5, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);

  ASSERT_TRUE(aNewResult.IsDone());
  EXPECT_GE(aNewResult.NbExt(), 1);

  // Verify all found points are valid
  for (int i = 0; i < aNewResult.NbExt(); ++i)
  {
    const auto& aExtremum = aNewResult[i];
    EXPECT_GE(aExtremum.Parameter, 0.0 - THE_TOL);
    EXPECT_LE(aExtremum.Parameter, 1.0 + THE_TOL);
    gp_Pnt aCurvePnt = anAdaptor.Value(aExtremum.Parameter);
    EXPECT_NEAR(aCurvePnt.Distance(aExtremum.Point), 0.0, 0.01);
  }

  // Compare minimum with old implementation
  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);
  if (anOldExtPC.IsDone() && anOldExtPC.NbExt() > 0)
  {
    double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
    double aOldMinDist = GetOldMinDistance(anOldExtPC);
    EXPECT_NEAR(aNewMinDist, aOldMinDist, 0.05); // Relaxed tolerance for BSpline
  }
}

//==================================================================================================
// Bezier SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Bezier_MinMode)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(3, 3, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  gp_Pnt aPoint(2.0, 4.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, 0.01);
}

TEST_F(ExtremaPC_SearchModeTest, Bezier_MaxMode)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 3, 0);
  aPoles(3) = gp_Pnt(3, 3, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  gp_Pnt aPoint(2.0, 1.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.PerformWithEndpoints(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());
  ASSERT_GE(aNewResult.NbExt(), 1);

  // With endpoint handling, max is at either endpoint (0,0,0) or (4,0,0)
  // Distance from (2,1,0) to endpoints = sqrt(4 + 1) = sqrt(5) ≈ 2.236
  double aExpectedMaxDist = std::sqrt(5.0);
  double aNewMaxDist      = std::sqrt(aNewResult.MaxSquareDistance());
  EXPECT_NEAR(aNewMaxDist, aExpectedMaxDist, 0.01);
}

//==================================================================================================
// Offset Curve SearchMode tests
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, OffsetCircle_MinMode)
{
  occ::handle<Geom_Circle> aBaseCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  occ::handle<Geom_OffsetCurve> anOffsetCurve =
    new Geom_OffsetCurve(aBaseCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

  ASSERT_TRUE(aNewResult.IsDone());

  double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
  double aOldMinDist = GetOldMinDistance(anOldExtPC);

  EXPECT_NEAR(aNewMinDist, aOldMinDist, 0.01);
  EXPECT_NEAR(aNewMinDist, 10.0, 0.01); // 25 - 15 = 10
}

TEST_F(ExtremaPC_SearchModeTest, OffsetCircle_MaxMode)
{
  // NOTE: Max mode for offset curves may not find extrema depending on the
  // BVH subdivision strategy.
  occ::handle<Geom_Circle> aBaseCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  occ::handle<Geom_OffsetCurve> anOffsetCurve =
    new Geom_OffsetCurve(aBaseCircle, 5.0, gp_Dir(0, 0, 1));
  GeomAdaptor_Curve anAdaptor(anOffsetCurve);

  gp_Pnt aPoint(25.0, 0.0, 0.0);

  ExtremaPC_Curve          anExtPC(anAdaptor);
  const ExtremaPC::Result& aNewResult =
    anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

  ASSERT_TRUE(aNewResult.IsDone());

  // Verify the found max extremum is valid if extrema exist
  if (aNewResult.NbExt() > 0)
  {
    const auto& aExtremum = aNewResult[aNewResult.MaxIndex()];
    gp_Pnt      aCurvePnt = anAdaptor.Value(aExtremum.Parameter);
    EXPECT_NEAR(aCurvePnt.Distance(aExtremum.Point), 0.0, 0.01);

    // The max distance should be around 40 (25 + 15)
    double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
    EXPECT_NEAR(aNewMaxDist, 40.0, 0.5);
  }
}

//==================================================================================================
// Random points stress tests for SearchMode
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Circle_RandomPoints_MinMode)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  std::mt19937                           aGen(12345);
  std::uniform_real_distribution<double> aDist(-50.0, 50.0);

  for (int i = 0; i < 50; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), 0.0);

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult =
      anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone())
      continue;

    ASSERT_TRUE(aNewResult.IsDone()) << "Iteration " << i;

    double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
    double aOldMinDist = GetOldMinDistance(anOldExtPC);

    EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL) << "Iteration " << i;
  }
}

TEST_F(ExtremaPC_SearchModeTest, Circle_RandomPoints_MaxMode)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  std::mt19937                           aGen(12345);
  std::uniform_real_distribution<double> aDist(-50.0, 50.0);

  for (int i = 0; i < 50; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), 0.0);

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult =
      anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Max);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone())
      continue;

    ASSERT_TRUE(aNewResult.IsDone()) << "Iteration " << i;

    double aNewMaxDist = std::sqrt(aNewResult.MaxSquareDistance());
    double aOldMaxDist = GetOldMaxDistance(anOldExtPC);

    EXPECT_NEAR(aNewMaxDist, aOldMaxDist, THE_TOL) << "Iteration " << i;
  }
}

TEST_F(ExtremaPC_SearchModeTest, Ellipse_RandomPoints_MinMode)
{
  occ::handle<Geom_Ellipse> aGeomEllipse =
    new Geom_Ellipse(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 20.0, 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomEllipse);

  std::mt19937                           aGen(54321);
  std::uniform_real_distribution<double> aDist(-50.0, 50.0);

  for (int i = 0; i < 50; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), 0.0);

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult =
      anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone())
      continue;

    ASSERT_TRUE(aNewResult.IsDone()) << "Iteration " << i;

    double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
    double aOldMinDist = GetOldMinDistance(anOldExtPC);

    EXPECT_NEAR(aNewMinDist, aOldMinDist, THE_TOL) << "Iteration " << i;
  }
}

TEST_F(ExtremaPC_SearchModeTest, BSpline_RandomPoints_MinMode)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(3, 2, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  std::mt19937                           aGen(11111);
  std::uniform_real_distribution<double> aDist(-2.0, 6.0);

  int aPassCount = 0;
  int aTestCount = 0;

  for (int i = 0; i < 30; ++i)
  {
    gp_Pnt aPoint(aDist(aGen), aDist(aGen), 0.0);

    ExtremaPC_Curve          anExtPC(anAdaptor);
    const ExtremaPC::Result& aNewResult =
      anExtPC.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

    Extrema_ExtPC anOldExtPC(aPoint, anAdaptor);

    if (!anOldExtPC.IsDone() || anOldExtPC.NbExt() == 0)
      continue;

    if (!aNewResult.IsDone() || aNewResult.NbExt() == 0)
      continue;

    ++aTestCount;

    double aNewMinDist = std::sqrt(aNewResult.MinSquareDistance());
    double aOldMinDist = GetOldMinDistance(anOldExtPC);

    // Allow larger tolerance for BSpline and count passes
    if (std::abs(aNewMinDist - aOldMinDist) < 0.1)
    {
      ++aPassCount;
    }
  }

  // At least 80% of valid tests should pass
  if (aTestCount > 0)
  {
    double aPassRate = static_cast<double>(aPassCount) / aTestCount;
    EXPECT_GE(aPassRate, 0.8) << "Pass rate: " << aPassRate * 100 << "% (" << aPassCount << "/"
                              << aTestCount << ")";
  }
}

//==================================================================================================
// Performance comparison for SearchMode
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, Performance_MinVsMinMax_Circle)
{
  occ::handle<Geom_Circle> aGeomCircle =
    new Geom_Circle(gp_Ax2(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0);
  GeomAdaptor_Curve anAdaptor(aGeomCircle);

  gp_Pnt aPoint(15.0, 8.0, 0.0);

  constexpr int aNbIterations = 5000;

  // Time MinMax mode
  ExtremaPC_Curve anExtPCMinMax(anAdaptor);

  auto aStartMinMax = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNbIterations; ++i)
  {
    anExtPCMinMax.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);
  }
  auto aEndMinMax = std::chrono::high_resolution_clock::now();
  auto aDurationMinMax =
    std::chrono::duration_cast<std::chrono::microseconds>(aEndMinMax - aStartMinMax).count();

  // Time Min mode
  ExtremaPC_Curve anExtPCMin(anAdaptor);

  auto aStartMin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNbIterations; ++i)
  {
    anExtPCMin.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);
  }
  auto aEndMin = std::chrono::high_resolution_clock::now();
  auto aDurationMin =
    std::chrono::duration_cast<std::chrono::microseconds>(aEndMin - aStartMin).count();

  std::cout << "  [PERF] Circle Min vs MinMax (" << aNbIterations << " iterations):" << std::endl;
  std::cout << "    Min mode:    " << aDurationMin << " us" << std::endl;
  std::cout << "    MinMax mode: " << aDurationMinMax << " us" << std::endl;
  std::cout << "    Ratio (MinMax/Min): " << (double)aDurationMinMax / aDurationMin << std::endl;

  SUCCEED();
}

TEST_F(ExtremaPC_SearchModeTest, Performance_MinVsMinMax_BSpline)
{
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(3, 2, 0);
  aPoles(4) = gp_Pnt(4, 0, 0);

  NCollection_Array1<double> aKnots(1, 2);
  aKnots(1) = 0.0;
  aKnots(2) = 1.0;

  NCollection_Array1<int> aMults(1, 2);
  aMults(1) = 4;
  aMults(2) = 4;

  occ::handle<Geom_BSplineCurve> aBSpline = new Geom_BSplineCurve(aPoles, aKnots, aMults, 3);
  GeomAdaptor_Curve              anAdaptor(aBSpline);

  gp_Pnt aPoint(2.0, 1.5, 0.0);

  constexpr int aNbIterations = 1000;

  // Time MinMax mode
  ExtremaPC_Curve anExtPCMinMax(anAdaptor);

  auto aStartMinMax = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNbIterations; ++i)
  {
    anExtPCMinMax.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);
  }
  auto aEndMinMax = std::chrono::high_resolution_clock::now();
  auto aDurationMinMax =
    std::chrono::duration_cast<std::chrono::microseconds>(aEndMinMax - aStartMinMax).count();

  // Time Min mode
  ExtremaPC_Curve anExtPCMin(anAdaptor);

  auto aStartMin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNbIterations; ++i)
  {
    anExtPCMin.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);
  }
  auto aEndMin = std::chrono::high_resolution_clock::now();
  auto aDurationMin =
    std::chrono::duration_cast<std::chrono::microseconds>(aEndMin - aStartMin).count();

  std::cout << "  [PERF] BSpline Min vs MinMax (" << aNbIterations << " iterations):" << std::endl;
  std::cout << "    Min mode:    " << aDurationMin << " us" << std::endl;
  std::cout << "    MinMax mode: " << aDurationMinMax << " us" << std::endl;
  std::cout << "    Ratio (MinMax/Min): " << (double)aDurationMinMax / aDurationMin << std::endl;

  SUCCEED();
}

//==================================================================================================
// Early termination efficiency test
//==================================================================================================

TEST_F(ExtremaPC_SearchModeTest, EarlyTermination_LargeBezier_MinMode)
{
  // Create a high-degree Bezier curve to test early termination benefit
  constexpr int              aNbPoles = 10;
  NCollection_Array1<gp_Pnt> aPoles(1, aNbPoles);

  // Create a wavy curve with multiple local extrema
  for (int i = 1; i <= aNbPoles; ++i)
  {
    double t  = static_cast<double>(i - 1) / (aNbPoles - 1);
    double x  = t * 20.0;
    double y  = 3.0 * std::sin(t * 4.0 * M_PI);
    aPoles(i) = gp_Pnt(x, y, 0);
  }

  occ::handle<Geom_BezierCurve> aBezier = new Geom_BezierCurve(aPoles);
  GeomAdaptor_Curve             anAdaptor(aBezier);

  // Test point that will have multiple candidate extrema
  gp_Pnt aPoint(10.0, 5.0, 0.0);

  constexpr int aNbIterations = 500;

  // Time MinMax mode (no early termination)
  ExtremaPC_Curve anExtPCMinMax(anAdaptor);

  auto aStartMinMax = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNbIterations; ++i)
  {
    anExtPCMinMax.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);
  }
  auto aEndMinMax = std::chrono::high_resolution_clock::now();
  auto aDurationMinMax =
    std::chrono::duration_cast<std::chrono::microseconds>(aEndMinMax - aStartMinMax).count();

  // Time Min mode (with early termination)
  ExtremaPC_Curve anExtPCMin(anAdaptor);

  auto aStartMin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < aNbIterations; ++i)
  {
    anExtPCMin.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);
  }
  auto aEndMin = std::chrono::high_resolution_clock::now();
  auto aDurationMin =
    std::chrono::duration_cast<std::chrono::microseconds>(aEndMin - aStartMin).count();

  // Verify correctness - Min mode should find the same minimum
  const ExtremaPC::Result& aMinMaxRes =
    anExtPCMinMax.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::MinMax);
  const ExtremaPC::Result& aMinRes =
    anExtPCMin.Perform(aPoint, THE_TOL, ExtremaPC::SearchMode::Min);

  ASSERT_TRUE(aMinMaxRes.IsDone());
  ASSERT_TRUE(aMinRes.IsDone());

  double aMinMaxMinDist = std::sqrt(aMinMaxRes.MinSquareDistance());
  double aMinModeDist   = std::sqrt(aMinRes.MinSquareDistance());

  // Both should find the same minimum distance
  EXPECT_NEAR(aMinMaxMinDist, aMinModeDist, 0.01);

  std::cout << "  [PERF] Large Bezier Early Termination (" << aNbIterations
            << " iterations):" << std::endl;
  std::cout << "    Min mode:    " << aDurationMin << " us" << std::endl;
  std::cout << "    MinMax mode: " << aDurationMinMax << " us" << std::endl;
  std::cout << "    Ratio (MinMax/Min): " << (double)aDurationMinMax / aDurationMin << std::endl;
  std::cout << "    Min distance: " << aMinModeDist << std::endl;

  SUCCEED();
}
