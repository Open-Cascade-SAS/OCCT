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
#include <ExtremaPS_OtherSurface.hxx>
#include <ExtremaPS_Surface.hxx>
#include <ExtremaPS_SurfaceOfRevolution.hxx>
#include <Extrema_ExtPS.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax1.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

#include <chrono>
#include <cmath>
#include <iostream>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 1.0e-9; // Must match or beat grid-based algorithm

//! Helper to compare implementations
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

  // Handle infinite solutions case
  if (aNewResult.Status == ExtremaPS::Status::InfiniteSolutions)
  {
    EXPECT_GE(aNewResult.InfiniteSquareDistance, 0.0)
      << theTestName << ": Infinite distance should be non-negative";
    return;
  }

  // New implementation must succeed
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

//! Create a simple meridian curve (line at radius 5 from axis)
occ::handle<Geom_Curve> MakeLineMeridian()
{
  gp_Pnt aStart(5.0, 0.0, -10.0);
  gp_Pnt anEnd(5.0, 0.0, 10.0);
  gp_Lin aLine(aStart, gp_Dir(anEnd.XYZ() - aStart.XYZ()));
  return new Geom_Line(aLine);
}

//! Create a circle meridian (torus-like)
occ::handle<Geom_Curve> MakeCircleMeridian()
{
  // Circle centered at (5, 0, 0) with radius 2, in the XZ plane
  gp_Ax2  anAxis(gp_Pnt(5.0, 0.0, 0.0), gp_Dir(0.0, 1.0, 0.0));
  gp_Circ aCircle(anAxis, 2.0);
  return new Geom_Circle(aCircle);
}

//! Create a curved meridian (BSpline)
occ::handle<Geom_BSplineCurve> MakeBSplineMeridian()
{
  // Create a curved profile
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles.SetValue(1, gp_Pnt(3.0, 0.0, -5.0));
  aPoles.SetValue(2, gp_Pnt(5.0, 0.0, -2.5));
  aPoles.SetValue(3, gp_Pnt(4.0, 0.0, 0.0));
  aPoles.SetValue(4, gp_Pnt(5.0, 0.0, 2.5));
  aPoles.SetValue(5, gp_Pnt(3.0, 0.0, 5.0));

  NCollection_Array1<double> aKnots(1, 3);
  aKnots.SetValue(1, 0.0);
  aKnots.SetValue(2, 0.5);
  aKnots.SetValue(3, 1.0);

  NCollection_Array1<int> aMults(1, 3);
  aMults.SetValue(1, 3);
  aMults.SetValue(2, 2);
  aMults.SetValue(3, 3);

  return new Geom_BSplineCurve(aPoles, aKnots, aMults, 2);
}
} // namespace

//==================================================================================================
// Test fixture for SurfaceOfRevolution
//==================================================================================================
class ExtremaPS_SurfaceOfRevolutionTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Create a surface of revolution by rotating a line around Z axis
    occ::handle<Geom_Curve> aMeridian = MakeLineMeridian();
    gp_Ax1                  anAxis(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1));
    myCylinderLike = new Geom_SurfaceOfRevolution(aMeridian, anAxis);
    myCylAdaptor.Load(myCylinderLike);

    // Create a torus-like surface
    occ::handle<Geom_Curve> aCircleMeridian = MakeCircleMeridian();
    myTorusLike                             = new Geom_SurfaceOfRevolution(aCircleMeridian, anAxis);
    myTorusAdaptor.Load(myTorusLike);

    // Create a surface with curved meridian
    occ::handle<Geom_BSplineCurve> aBSplineMeridian = MakeBSplineMeridian();
    myCurvedSurface = new Geom_SurfaceOfRevolution(aBSplineMeridian, anAxis);
    myCurvedAdaptor.Load(myCurvedSurface);
  }

  occ::handle<Geom_SurfaceOfRevolution> myCylinderLike;
  occ::handle<Geom_SurfaceOfRevolution> myTorusLike;
  occ::handle<Geom_SurfaceOfRevolution> myCurvedSurface;
  GeomAdaptor_Surface                   myCylAdaptor;
  GeomAdaptor_Surface                   myTorusAdaptor;
  GeomAdaptor_Surface                   myCurvedAdaptor;
};

//==================================================================================================
// Basic Tests - Cylinder-like surface
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, CylinderLike_PointOutside)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointOutside");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, CylinderLike_PointInside)
{
  gp_Pnt aP(2.0, 0.0, 0.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointInside");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, CylinderLike_PointOnAxis)
{
  gp_Pnt aP(0.0, 0.0, 5.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointOnAxis");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, CylinderLike_PointDiagonal)
{
  gp_Pnt aP(7.0, 7.0, 3.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointDiagonal");
}

//==================================================================================================
// Torus-like surface tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, TorusLike_PointOutside)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myTorusAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "TorusLike_PointOutside");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, TorusLike_PointInHole)
{
  gp_Pnt aP(0.0, 0.0, 0.0);
  CompareMinDistances(aP, myTorusAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "TorusLike_PointInHole");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, TorusLike_PointAbove)
{
  gp_Pnt aP(5.0, 0.0, 5.0);
  CompareMinDistances(aP, myTorusAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "TorusLike_PointAbove");
}

//==================================================================================================
// Curved meridian tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, Curved_PointNear)
{
  gp_Pnt aP(6.0, 0.0, 0.0);
  CompareMinDistances(aP, myCurvedAdaptor, 0.0, 2 * M_PI, 0.0, 1.0, "Curved_PointNear");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, Curved_PointFar)
{
  gp_Pnt aP(15.0, 0.0, 0.0);
  CompareMinDistances(aP, myCurvedAdaptor, 0.0, 2 * M_PI, 0.0, 1.0, "Curved_PointFar");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, Curved_PointDiagonal)
{
  // For curved meridians at diagonal points, compare against grid-based OtherSurface
  // The specialized algorithm matches OtherSurface which is our new baseline
  gp_Pnt                        aP(5.0, 5.0, 2.0);
  const ExtremaPS::Domain2D     aDomain(0.0, 2 * M_PI, 0.0, 1.0);
  ExtremaPS_OtherSurface        aGridExtPS(myCurvedSurface, aDomain);
  ExtremaPS_SurfaceOfRevolution aSpecExtPS(myCurvedSurface, aDomain);
  const ExtremaPS::Result&      aGridResult = aGridExtPS.PerformWithBoundary(aP, THE_TOLERANCE);
  const ExtremaPS::Result&      aSpecResult = aSpecExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aGridResult.Status, ExtremaPS::Status::OK);
  ASSERT_EQ(aSpecResult.Status, ExtremaPS::Status::OK);

  double aGridMin = aGridResult.MinSquareDistance();
  double aSpecMin = aSpecResult.MinSquareDistance();
  EXPECT_NEAR(std::sqrt(aGridMin), std::sqrt(aSpecMin), THE_DIST_TOLERANCE)
    << "Curved_PointDiagonal: Specialized should match grid-based";
}

//==================================================================================================
// Partial domain tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, PartialU_HalfCircle)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, M_PI, -10.0, 10.0, "PartialU_HalfCircle");
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, PartialV_TopHalf)
{
  gp_Pnt aP(10.0, 0.0, 5.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, 0.0, 10.0, "PartialV_TopHalf");
}

//==================================================================================================
// Direct API tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, DirectAPI_Basic)
{
  ExtremaPS_SurfaceOfRevolution anExtPS(myCylinderLike);

  gp_Pnt                   aP(10.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.NbExt(), 1);

  // Closest point should be at radius 5
  double aExpectedDist = 5.0;
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedDist, THE_DIST_TOLERANCE);
}

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, DirectAPI_OnAxis_InfiniteSolutions)
{
  ExtremaPS_SurfaceOfRevolution anExtPS(myCylinderLike);

  gp_Pnt                   aP(0.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aP, THE_TOLERANCE);

  // Point on axis should give infinite solutions
  EXPECT_EQ(aResult.Status, ExtremaPS::Status::InfiniteSolutions);
  EXPECT_NEAR(std::sqrt(aResult.InfiniteSquareDistance), 5.0, THE_DIST_TOLERANCE);
}

//==================================================================================================
// Performance comparison test
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, Performance_Comparison)
{
  const int aNumIterations = 1000;

  // Generate test points
  std::vector<gp_Pnt> aPoints;
  for (int i = 0; i < aNumIterations; ++i)
  {
    double aAngle = (i * 2.0 * M_PI) / aNumIterations;
    double aR     = 3.0 + 10.0 * (i % 10) / 10.0;
    double aZ     = -5.0 + 10.0 * (i % 20) / 20.0;
    aPoints.push_back(gp_Pnt(aR * std::cos(aAngle), aR * std::sin(aAngle), aZ));
  }

  // Time old implementation
  auto aOldStart = std::chrono::high_resolution_clock::now();
  for (const gp_Pnt& aP : aPoints)
  {
    Extrema_ExtPS
      anExtPS(aP, myCurvedAdaptor, 0.0, 2 * M_PI, 0.0, 1.0, THE_TOLERANCE, THE_TOLERANCE);
    (void)anExtPS.IsDone();
  }
  auto aOldEnd = std::chrono::high_resolution_clock::now();
  auto aOldDuration =
    std::chrono::duration_cast<std::chrono::microseconds>(aOldEnd - aOldStart).count();

  // Time new implementation
  ExtremaPS_Surface aNewExtPS(myCurvedAdaptor, ExtremaPS::Domain2D(0.0, 2 * M_PI, 0.0, 1.0));
  auto              aNewStart = std::chrono::high_resolution_clock::now();
  for (const gp_Pnt& aP : aPoints)
  {
    (void)aNewExtPS.Perform(aP, THE_TOLERANCE);
  }
  auto aNewEnd = std::chrono::high_resolution_clock::now();
  auto aNewDuration =
    std::chrono::duration_cast<std::chrono::microseconds>(aNewEnd - aNewStart).count();

  double aSpeedup = static_cast<double>(aOldDuration) / aNewDuration;

  std::cout << "[          ] SurfaceOfRevolution Performance (" << aNumIterations
            << " iterations):\n";
  std::cout << "[          ]   Old: " << aOldDuration << " us\n";
  std::cout << "[          ]   New: " << aNewDuration << " us\n";
  std::cout << "[          ]   Speedup: " << aSpeedup << "x\n";

  // We expect the new implementation to be faster
  EXPECT_GT(aSpeedup, 1.0) << "New implementation should be faster";
}

//==================================================================================================
// Stress test - compare specialized revolution vs grid-based OtherSurface
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfRevolutionTest, StressTest_RandomPoints)
{
  int    aPassCount  = 0;
  int    aTotalCount = 0;
  int    aFailCount  = 0;
  double aMaxDiff    = 0.0;

  const ExtremaPS::Domain2D aDomain(0.0, 2 * M_PI, -10.0, 10.0);

  // Use grid-based OtherSurface as reference (most accurate)
  ExtremaPS_OtherSurface aGridExtPS(myCylinderLike, aDomain);

  // Specialized revolution implementation
  ExtremaPS_SurfaceOfRevolution aSpecExtPS(myCylinderLike, aDomain);

  for (int angle = 0; angle < 360; angle += 30)
  {
    double aRad = angle * M_PI / 180.0;
    for (double r = 1.0; r <= 15.0; r += 2.0)
    {
      for (double z = -8.0; z <= 8.0; z += 4.0)
      {
        gp_Pnt aP(r * std::cos(aRad), r * std::sin(aRad), z);

        const ExtremaPS::Result& aGridResult = aGridExtPS.PerformWithBoundary(aP, THE_TOLERANCE);
        const ExtremaPS::Result& aSpecResult = aSpecExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

        ++aTotalCount;

        if (aSpecResult.Status == ExtremaPS::Status::InfiniteSolutions)
        {
          ++aPassCount;
          continue;
        }

        if (aGridResult.Status == ExtremaPS::Status::OK
            && aSpecResult.Status == ExtremaPS::Status::OK)
        {
          double aGridMin = aGridResult.MinSquareDistance();
          double aSpecMin = aSpecResult.MinSquareDistance();

          double aDiff = std::abs(std::sqrt(aGridMin) - std::sqrt(aSpecMin));
          aMaxDiff     = std::max(aMaxDiff, aDiff);

          // Pass if specialized result is close to or better than grid result
          if (aDiff < THE_DIST_TOLERANCE || aSpecMin <= aGridMin)
          {
            ++aPassCount;
          }
          else if (aFailCount < 5)
          {
            // Print first few failures for debugging
            std::cout << "[          ] FAIL P=(" << aP.X() << "," << aP.Y() << "," << aP.Z() << ")"
                      << " grid=" << std::sqrt(aGridMin) << " spec=" << std::sqrt(aSpecMin)
                      << " diff=" << aDiff << std::endl;
            ++aFailCount;
          }
        }
      }
    }
  }

  std::cout << "[          ] Max difference (spec vs grid): " << aMaxDiff << std::endl;
  EXPECT_EQ(aPassCount, aTotalCount) << "Some points failed comparison";
}
