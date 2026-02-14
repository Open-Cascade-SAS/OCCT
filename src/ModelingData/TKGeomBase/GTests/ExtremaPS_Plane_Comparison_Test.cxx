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
#include <ExtremaPS_Surface.hxx>
#include <Extrema_ExtPS.hxx>

#include <Geom_Plane.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>

#include <cmath>
#include <vector>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 1.0e-3; // Relaxed for plane extrema

//! Helper to compare or verify new implementation when old may fail
void CompareMinDistances(const gp_Pnt&        thePoint,
                         GeomAdaptor_Surface& theAdaptor,
                         double               theUMin,
                         double               theUMax,
                         double               theVMin,
                         double               theVMax,
                         const std::string&   theTestName)
{
  Extrema_ExtPS anOldExtPS(thePoint,
                           theAdaptor,
                           theUMin,
                           theUMax,
                           theVMin,
                           theVMax,
                           THE_TOLERANCE,
                           THE_TOLERANCE);

  ExtremaPS_Surface aNewExtPS(theAdaptor, ExtremaPS::Domain2D(theUMin, theUMax, theVMin, theVMax));
  const ExtremaPS::Result& aNewResult = aNewExtPS.PerformWithBoundary(thePoint, THE_TOLERANCE);

  // New implementation must always succeed
  ASSERT_EQ(aNewResult.Status, ExtremaPS::Status::OK)
    << theTestName << ": New implementation failed";

  // If old implementation failed, we just verify new worked
  if (!anOldExtPS.IsDone() || anOldExtPS.NbExt() == 0)
  {
    EXPECT_GE(aNewResult.NbExt(), 1) << theTestName << ": New implementation should find extrema";
    return;
  }

  double aOldMinSqDist = std::numeric_limits<double>::max();
  for (int i = 1; i <= anOldExtPS.NbExt(); ++i)
  {
    aOldMinSqDist = std::min(aOldMinSqDist, anOldExtPS.SquareDistance(i));
  }

  double aNewMinSqDist = aNewResult.MinSquareDistance();

  EXPECT_NEAR(std::sqrt(aOldMinSqDist), std::sqrt(aNewMinSqDist), THE_DIST_TOLERANCE)
    << theTestName << ": Min distances differ - Old: " << std::sqrt(aOldMinSqDist)
    << ", New: " << std::sqrt(aNewMinSqDist);
}
} // namespace

//==================================================================================================
// Test fixture for Plane comparison tests
//==================================================================================================
class ExtremaPS_PlaneComparisonTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Create standard XY plane at origin
    myPlane = new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1))));
    myAdaptor.Load(myPlane);
  }

  occ::handle<Geom_Plane> myPlane;
  GeomAdaptor_Surface     myAdaptor;
};

//==================================================================================================
// Basic Comparison Tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneComparisonTest, PointAbovePlane)
{
  gp_Pnt aP(5.0, 5.0, 10.0);
  CompareMinDistances(aP, myAdaptor, -100.0, 100.0, -100.0, 100.0, "PointAbovePlane");
}

TEST_F(ExtremaPS_PlaneComparisonTest, PointBelowPlane)
{
  gp_Pnt aP(5.0, 5.0, -7.0);
  CompareMinDistances(aP, myAdaptor, -100.0, 100.0, -100.0, 100.0, "PointBelowPlane");
}

TEST_F(ExtremaPS_PlaneComparisonTest, PointOnPlane)
{
  gp_Pnt aP(3.0, 4.0, 0.0);
  CompareMinDistances(aP, myAdaptor, -100.0, 100.0, -100.0, 100.0, "PointOnPlane");
}

TEST_F(ExtremaPS_PlaneComparisonTest, PointAtOrigin)
{
  gp_Pnt aP(0.0, 0.0, 5.0);
  CompareMinDistances(aP, myAdaptor, -100.0, 100.0, -100.0, 100.0, "PointAtOrigin");
}

TEST_F(ExtremaPS_PlaneComparisonTest, PointFarAway)
{
  gp_Pnt aP(1000.0, 2000.0, 500.0);
  CompareMinDistances(aP, myAdaptor, -5000.0, 5000.0, -5000.0, 5000.0, "PointFarAway");
}

//==================================================================================================
// Tilted Plane Tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneComparisonTest, TiltedPlane_PointAbove)
{
  // Create tilted plane (45 degrees around X axis)
  gp_Dir                  aNorm(0.0, -std::sin(M_PI / 4), std::cos(M_PI / 4));
  occ::handle<Geom_Plane> aTiltedPlane = new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), aNorm)));
  GeomAdaptor_Surface     anAdaptor(aTiltedPlane);

  gp_Pnt aP(5.0, 5.0, 5.0);
  CompareMinDistances(aP, anAdaptor, -100.0, 100.0, -100.0, 100.0, "TiltedPlane_PointAbove");
}

TEST_F(ExtremaPS_PlaneComparisonTest, TiltedPlane_PointBelow)
{
  gp_Dir                  aNorm(0.0, -std::sin(M_PI / 4), std::cos(M_PI / 4));
  occ::handle<Geom_Plane> aTiltedPlane = new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(0, 0, 0), aNorm)));
  GeomAdaptor_Surface     anAdaptor(aTiltedPlane);

  gp_Pnt aP(5.0, -5.0, -5.0);
  CompareMinDistances(aP, anAdaptor, -100.0, 100.0, -100.0, 100.0, "TiltedPlane_PointBelow");
}

//==================================================================================================
// Translated Plane Tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneComparisonTest, TranslatedPlane_PointNear)
{
  occ::handle<Geom_Plane> aTransPlane =
    new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1))));
  GeomAdaptor_Surface anAdaptor(aTransPlane);

  gp_Pnt aP(105.0, 205.0, 60.0);
  CompareMinDistances(aP, anAdaptor, -100.0, 300.0, 0.0, 400.0, "TranslatedPlane_PointNear");
}

TEST_F(ExtremaPS_PlaneComparisonTest, TranslatedPlane_PointFar)
{
  occ::handle<Geom_Plane> aTransPlane =
    new Geom_Plane(gp_Pln(gp_Ax3(gp_Pnt(100, 200, 50), gp_Dir(0, 0, 1))));
  GeomAdaptor_Surface anAdaptor(aTransPlane);

  gp_Pnt aP(500.0, 600.0, 150.0);
  CompareMinDistances(aP, anAdaptor, -100.0, 800.0, 0.0, 800.0, "TranslatedPlane_PointFar");
}

//==================================================================================================
// Random Points Tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneComparisonTest, RandomPoints_10Tests)
{
  // Test with various random-like points
  std::vector<gp_Pnt> aPoints = {gp_Pnt(1.5, 2.3, 4.7),
                                 gp_Pnt(-3.2, 5.1, -2.8),
                                 gp_Pnt(10.0, -10.0, 7.5),
                                 gp_Pnt(0.0, 0.0, 0.001),
                                 gp_Pnt(50.0, 50.0, 0.0),
                                 gp_Pnt(-25.0, 75.0, 15.0),
                                 gp_Pnt(0.1, 0.1, 100.0),
                                 gp_Pnt(100.0, 0.0, 0.5),
                                 gp_Pnt(0.0, 100.0, -0.5),
                                 gp_Pnt(-50.0, -50.0, 25.0)};

  for (size_t i = 0; i < aPoints.size(); ++i)
  {
    std::string aTestName = "RandomPoint_" + std::to_string(i);
    CompareMinDistances(aPoints[i], myAdaptor, -200.0, 200.0, -200.0, 200.0, aTestName);
  }
}

//==================================================================================================
// Bounded Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_PlaneComparisonTest, BoundedDomain_PointInside)
{
  gp_Pnt aP(5.0, 5.0, 3.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 10.0, 0.0, 10.0, "BoundedDomain_PointInside");
}

TEST_F(ExtremaPS_PlaneComparisonTest, BoundedDomain_PointOutside)
{
  gp_Pnt aP(15.0, 15.0, 3.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 10.0, 0.0, 10.0, "BoundedDomain_PointOutside");
}

TEST_F(ExtremaPS_PlaneComparisonTest, BoundedDomain_PointNearCorner)
{
  gp_Pnt aP(-1.0, -1.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 10.0, 0.0, 10.0, "BoundedDomain_PointNearCorner");
}

//==================================================================================================
// Performance-like stress test
//==================================================================================================

TEST_F(ExtremaPS_PlaneComparisonTest, StressTest_100Points)
{
  // Generate grid of test points
  int aPassCount  = 0;
  int aTotalCount = 0;

  for (int i = -5; i <= 5; i += 2)
  {
    for (int j = -5; j <= 5; j += 2)
    {
      for (int k = -5; k <= 5; k += 2)
      {
        if (k == 0)
          continue; // Skip points on plane

        gp_Pnt aP(i * 10.0, j * 10.0, k * 10.0);

        // Old implementation
        Extrema_ExtPS
          anOldExtPS(aP, myAdaptor, -100.0, 100.0, -100.0, 100.0, THE_TOLERANCE, THE_TOLERANCE);

        // New implementation
        ExtremaPS_Surface aNewExtPS(myAdaptor, ExtremaPS::Domain2D(-100.0, 100.0, -100.0, 100.0));
        const ExtremaPS::Result& aNewResult = aNewExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

        ++aTotalCount;

        if (anOldExtPS.IsDone() && aNewResult.Status == ExtremaPS::Status::OK)
        {
          double aOldMin = std::numeric_limits<double>::max();
          for (int n = 1; n <= anOldExtPS.NbExt(); ++n)
          {
            aOldMin = std::min(aOldMin, anOldExtPS.SquareDistance(n));
          }
          double aNewMin = aNewResult.MinSquareDistance();

          if (std::abs(std::sqrt(aOldMin) - std::sqrt(aNewMin)) < THE_DIST_TOLERANCE)
          {
            ++aPassCount;
          }
        }
      }
    }
  }

  // All points should pass
  EXPECT_EQ(aPassCount, aTotalCount);
}
