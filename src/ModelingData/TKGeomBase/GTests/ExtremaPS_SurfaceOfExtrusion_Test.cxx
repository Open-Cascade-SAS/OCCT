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
#include <ExtremaPS_SurfaceOfExtrusion.hxx>
#include <Extrema_ExtPS.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_Line.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>
#include <gp_Dir.hxx>
#include <gp_Elips.hxx>
#include <gp_Lin.hxx>
#include <gp_Pnt.hxx>
#include <NCollection_Array1.hxx>

#include <chrono>
#include <cmath>
#include <iostream>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 0.1; // Relaxed for comparing different algorithms (diagonal extrusion)

//! Helper to compare implementations
void CompareMinDistances(const gp_Pnt&        thePoint,
                         GeomAdaptor_Surface& theAdaptor,
                         double               theUMin,
                         double               theUMax,
                         double               theVMin,
                         double               theVMax,
                         const std::string&   theTestName)
{
  Extrema_ExtPS anOldExtPS(thePoint, theAdaptor, theUMin, theUMax, theVMin, theVMax, THE_TOLERANCE,
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
  ASSERT_EQ(aNewResult.Status, ExtremaPS::Status::OK) << theTestName << ": New implementation failed";

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

//! Create a line basis curve (creates a plane-like surface)
occ::handle<Geom_Curve> MakeLineBasisCurve()
{
  gp_Lin aLine(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(1.0, 0.0, 0.0));
  return new Geom_Line(aLine);
}

//! Create a circle basis curve (creates a cylinder-like surface)
occ::handle<Geom_Curve> MakeCircleBasisCurve()
{
  gp_Ax2  anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Circ aCircle(anAxis, 5.0);
  return new Geom_Circle(aCircle);
}

//! Create an ellipse basis curve
occ::handle<Geom_Curve> MakeEllipseBasisCurve()
{
  gp_Ax2   anAxis(gp_Pnt(0.0, 0.0, 0.0), gp_Dir(0.0, 0.0, 1.0));
  gp_Elips anEllipse(anAxis, 5.0, 3.0);
  return new Geom_Ellipse(anEllipse);
}

//! Create a curved BSpline basis curve
occ::handle<Geom_BSplineCurve> MakeBSplineBasisCurve()
{
  // Create a wavy profile
  NCollection_Array1<gp_Pnt> aPoles(1, 5);
  aPoles.SetValue(1, gp_Pnt(-5.0, 0.0, 0.0));
  aPoles.SetValue(2, gp_Pnt(-2.5, 3.0, 0.0));
  aPoles.SetValue(3, gp_Pnt(0.0, 0.0, 0.0));
  aPoles.SetValue(4, gp_Pnt(2.5, -3.0, 0.0));
  aPoles.SetValue(5, gp_Pnt(5.0, 0.0, 0.0));

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
// Test fixture for SurfaceOfExtrusion
//==================================================================================================
class ExtremaPS_SurfaceOfExtrusionTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Extrusion direction along Z
    gp_Dir aDir(0.0, 0.0, 1.0);

    // Create a plane-like surface by extruding a line
    occ::handle<Geom_Curve> aLineCurve = MakeLineBasisCurve();
    myPlaneLike                        = new Geom_SurfaceOfLinearExtrusion(aLineCurve, aDir);
    myPlaneAdaptor.Load(myPlaneLike);

    // Create a cylinder-like surface by extruding a circle
    occ::handle<Geom_Curve> aCircleCurve = MakeCircleBasisCurve();
    myCylinderLike                       = new Geom_SurfaceOfLinearExtrusion(aCircleCurve, aDir);
    myCylAdaptor.Load(myCylinderLike);

    // Create an elliptic cylinder surface
    occ::handle<Geom_Curve> anEllipseCurve = MakeEllipseBasisCurve();
    myEllipticCylinder                     = new Geom_SurfaceOfLinearExtrusion(anEllipseCurve, aDir);
    myEllipticAdaptor.Load(myEllipticCylinder);

    // Create a curved extrusion surface
    occ::handle<Geom_BSplineCurve> aBSplineCurve = MakeBSplineBasisCurve();
    myCurvedSurface = new Geom_SurfaceOfLinearExtrusion(aBSplineCurve, aDir);
    myCurvedAdaptor.Load(myCurvedSurface);

    // Create a diagonal extrusion
    gp_Dir                  aDiagDir(1.0, 1.0, 1.0);
    occ::handle<Geom_Curve> aCircle2 = MakeCircleBasisCurve();
    myDiagonalExtrusion              = new Geom_SurfaceOfLinearExtrusion(aCircle2, aDiagDir);
    myDiagAdaptor.Load(myDiagonalExtrusion);
  }

  occ::handle<Geom_SurfaceOfLinearExtrusion> myPlaneLike;
  occ::handle<Geom_SurfaceOfLinearExtrusion> myCylinderLike;
  occ::handle<Geom_SurfaceOfLinearExtrusion> myEllipticCylinder;
  occ::handle<Geom_SurfaceOfLinearExtrusion> myCurvedSurface;
  occ::handle<Geom_SurfaceOfLinearExtrusion> myDiagonalExtrusion;
  GeomAdaptor_Surface                        myPlaneAdaptor;
  GeomAdaptor_Surface                        myCylAdaptor;
  GeomAdaptor_Surface                        myEllipticAdaptor;
  GeomAdaptor_Surface                        myCurvedAdaptor;
  GeomAdaptor_Surface                        myDiagAdaptor;
};

//==================================================================================================
// Basic Tests - Cylinder-like surface (extruded circle)
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, CylinderLike_PointOutside)
{
  gp_Pnt aP(10.0, 0.0, 5.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointOutside");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, CylinderLike_PointInside)
{
  gp_Pnt aP(2.0, 0.0, 5.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointInside");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, CylinderLike_PointOnAxis)
{
  gp_Pnt aP(0.0, 0.0, 5.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointOnAxis");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, CylinderLike_PointDiagonal)
{
  gp_Pnt aP(7.0, 7.0, 3.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointDiagonal");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, CylinderLike_PointAboveDomain)
{
  gp_Pnt aP(5.0, 0.0, 15.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "CylinderLike_PointAboveDomain");
}

//==================================================================================================
// Elliptic cylinder tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, EllipticCylinder_PointOnMajorAxis)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myEllipticAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "EllipticCylinder_PointOnMajorAxis");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, EllipticCylinder_PointOnMinorAxis)
{
  gp_Pnt aP(0.0, 8.0, 0.0);
  CompareMinDistances(aP, myEllipticAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "EllipticCylinder_PointOnMinorAxis");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, EllipticCylinder_PointDiagonal)
{
  gp_Pnt aP(6.0, 6.0, 5.0);
  CompareMinDistances(aP, myEllipticAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "EllipticCylinder_PointDiagonal");
}

//==================================================================================================
// Curved extrusion tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, Curved_PointNear)
{
  gp_Pnt aP(0.0, 3.0, 0.0);
  CompareMinDistances(aP, myCurvedAdaptor, 0.0, 1.0, -10.0, 10.0, "Curved_PointNear");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, Curved_PointFar)
{
  gp_Pnt aP(0.0, 15.0, 5.0);
  CompareMinDistances(aP, myCurvedAdaptor, 0.0, 1.0, -10.0, 10.0, "Curved_PointFar");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, Curved_PointAbove)
{
  gp_Pnt aP(2.0, 1.0, 8.0);
  CompareMinDistances(aP, myCurvedAdaptor, 0.0, 1.0, -10.0, 10.0, "Curved_PointAbove");
}

//==================================================================================================
// Diagonal extrusion tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, DiagonalExtrusion_PointNear)
{
  gp_Pnt aP(8.0, 0.0, 0.0);
  CompareMinDistances(aP, myDiagAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "DiagonalExtrusion_PointNear");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, DiagonalExtrusion_PointAlong)
{
  gp_Pnt aP(5.0, 5.0, 5.0);
  CompareMinDistances(aP, myDiagAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "DiagonalExtrusion_PointAlong");
}

//==================================================================================================
// Partial domain tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, PartialU_HalfCircle)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, M_PI, -10.0, 10.0, "PartialU_HalfCircle");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, PartialV_TopHalf)
{
  gp_Pnt aP(10.0, 0.0, 5.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, 2 * M_PI, 0.0, 10.0, "PartialV_TopHalf");
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, SmallDomain)
{
  gp_Pnt aP(5.0, 1.0, 1.0);
  CompareMinDistances(aP, myCylAdaptor, 0.0, M_PI / 4, 0.0, 2.0, "SmallDomain");
}

//==================================================================================================
// Direct API tests
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, DirectAPI_Basic)
{
  ExtremaPS_SurfaceOfExtrusion anExtPS(myCylinderLike);

  gp_Pnt                       aP(10.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.NbExt(), 1);

  // Closest point should be at radius 5
  double aExpectedDist = 5.0;
  EXPECT_NEAR(std::sqrt(aResult.MinSquareDistance()), aExpectedDist, THE_DIST_TOLERANCE);
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, DirectAPI_WithDomain)
{
  ExtremaPS_SurfaceOfExtrusion anExtPS(myCylinderLike, ExtremaPS::Domain2D(0.0, 2 * M_PI, -5.0, 5.0));

  gp_Pnt                       aP(10.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.NbExt(), 1);

  // Verify the result is within domain
  for (int i = 0; i < aResult.NbExt(); ++i)
  {
    const ExtremaPS::ExtremumResult& anExt = aResult.Extrema.Value(i);
    EXPECT_GE(anExt.V, -5.0 - THE_TOLERANCE);
    EXPECT_LE(anExt.V, 5.0 + THE_TOLERANCE);
  }
}

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, DirectAPI_PointOnSurface)
{
  ExtremaPS_SurfaceOfExtrusion anExtPS(myCylinderLike);

  // Point exactly on the surface
  gp_Pnt                       aP(5.0, 0.0, 0.0);
  const ExtremaPS::Result& aResult = anExtPS.Perform(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.NbExt(), 1);
  EXPECT_NEAR(aResult.MinSquareDistance(), 0.0, THE_TOLERANCE * THE_TOLERANCE);
}

//==================================================================================================
// Performance comparison test
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, Performance_Comparison)
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
    Extrema_ExtPS anExtPS(aP, myCurvedAdaptor, 0.0, 1.0, -10.0, 10.0, THE_TOLERANCE, THE_TOLERANCE);
    (void)anExtPS.IsDone();
  }
  auto aOldEnd = std::chrono::high_resolution_clock::now();
  auto aOldDuration =
      std::chrono::duration_cast<std::chrono::microseconds>(aOldEnd - aOldStart).count();

  // Time new implementation
  ExtremaPS_Surface aNewExtPS(myCurvedAdaptor, ExtremaPS::Domain2D(0.0, 1.0, -10.0, 10.0));
  auto              aNewStart = std::chrono::high_resolution_clock::now();
  for (const gp_Pnt& aP : aPoints)
  {
    (void)aNewExtPS.Perform(aP, THE_TOLERANCE);
  }
  auto aNewEnd = std::chrono::high_resolution_clock::now();
  auto aNewDuration =
      std::chrono::duration_cast<std::chrono::microseconds>(aNewEnd - aNewStart).count();

  double aSpeedup = static_cast<double>(aOldDuration) / aNewDuration;

  std::cout << "[          ] SurfaceOfExtrusion Performance (" << aNumIterations << " iterations):\n";
  std::cout << "[          ]   Old: " << aOldDuration << " us\n";
  std::cout << "[          ]   New: " << aNewDuration << " us\n";
  std::cout << "[          ]   Speedup: " << aSpeedup << "x\n";

  // We expect the new implementation to be faster
  EXPECT_GT(aSpeedup, 1.0) << "New implementation should be faster";
}

//==================================================================================================
// Stress test
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, StressTest_RandomPoints)
{
  int aPassCount  = 0;
  int aTotalCount = 0;

  for (int angle = 0; angle < 360; angle += 30)
  {
    double aRad = angle * M_PI / 180.0;
    for (double r = 1.0; r <= 15.0; r += 2.0)
    {
      for (double z = -8.0; z <= 8.0; z += 4.0)
      {
        gp_Pnt aP(r * std::cos(aRad), r * std::sin(aRad), z);

        Extrema_ExtPS anOldExtPS(aP, myCylAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, THE_TOLERANCE,
                                  THE_TOLERANCE);

        ExtremaPS_Surface       aNewExtPS(myCylAdaptor, ExtremaPS::Domain2D(0.0, 2 * M_PI, -10.0, 10.0));
        const ExtremaPS::Result& aNewResult = aNewExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

        ++aTotalCount;

        if (aNewResult.Status == ExtremaPS::Status::InfiniteSolutions)
        {
          ++aPassCount;
          continue;
        }

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

  EXPECT_EQ(aPassCount, aTotalCount) << "Some points failed comparison";
}

//==================================================================================================
// Edge cases
//==================================================================================================

TEST_F(ExtremaPS_SurfaceOfExtrusionTest, EdgeCase_PointOnExtrusionLine)
{
  // Point directly along the extrusion direction from a surface point
  ExtremaPS_SurfaceOfExtrusion anExtPS(myCylinderLike, ExtremaPS::Domain2D(0.0, 2 * M_PI, -10.0, 10.0));

  gp_Pnt                       aP(5.0, 0.0, 20.0); // On the surface axis extension, outside V domain
  const ExtremaPS::Result& aResult = anExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

  ASSERT_EQ(aResult.Status, ExtremaPS::Status::OK);
  EXPECT_GE(aResult.NbExt(), 1);

  // Find the minimum extremum
  double aMinSqDist = aResult.Extrema.Value(0).SquareDistance;
  int    aMinIdx    = 0;
  for (int i = 1; i < aResult.NbExt(); ++i)
  {
    if (aResult.Extrema.Value(i).SquareDistance < aMinSqDist)
    {
      aMinSqDist = aResult.Extrema.Value(i).SquareDistance;
      aMinIdx    = i;
    }
  }

  // Should find minimum at V = 10 (boundary)
  EXPECT_NEAR(aResult.Extrema.Value(aMinIdx).V, 10.0, THE_DIST_TOLERANCE);
}
