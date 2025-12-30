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

#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Torus.hxx>

#include <cmath>
#include <vector>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 1.0e-3; // Relaxed for torus extrema

//! Helper to compare or verify new implementation when old may fail
void CompareMinDistances(const gp_Pnt&        thePoint,
                         GeomAdaptor_Surface& theAdaptor,
                         double               theUMin,
                         double               theUMax,
                         double               theVMin,
                         double               theVMax,
                         const std::string&   theTestName)
{
  Extrema_ExtPS anOldExtPS(thePoint, theAdaptor, theUMin, theUMax, theVMin, theVMax,
                           THE_TOLERANCE, THE_TOLERANCE);

  ExtremaPS_Surface aNewExtPS(theAdaptor, ExtremaPS::Domain2D(theUMin, theUMax, theVMin, theVMax));
  const ExtremaPS::Result& aNewResult =aNewExtPS.PerformWithBoundary(thePoint, THE_TOLERANCE);

  // Handle infinite solutions case (e.g., point on torus axis)
  if (aNewResult.Status == ExtremaPS::Status::InfiniteSolutions)
  {
    EXPECT_GE(aNewResult.InfiniteSquareDistance, 0.0) << theTestName << ": Infinite distance should be non-negative";
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
} // namespace

//==================================================================================================
// Test fixture
//==================================================================================================
class ExtremaPS_TorusComparisonTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Torus with major radius 10, minor radius 3
    gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 3.0);
    myTorus = new Geom_ToroidalSurface(aTorus);
    myAdaptor.Load(myTorus);
  }

  Handle(Geom_ToroidalSurface) myTorus;
  GeomAdaptor_Surface          myAdaptor;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusComparisonTest, PointOutsideTorus)
{
  gp_Pnt aP(20.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "PointOutsideTorus");
}

TEST_F(ExtremaPS_TorusComparisonTest, PointInsideHole)
{
  gp_Pnt aP(3.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "PointInsideHole");
}

TEST_F(ExtremaPS_TorusComparisonTest, PointInsideTube)
{
  // Point inside the tube (between R-r and R+r radially, at the torus centerline)
  gp_Pnt aP(10.0, 0.0, 1.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "PointInsideTube");
}

TEST_F(ExtremaPS_TorusComparisonTest, PointOnSurface)
{
  // Point on outer equator of torus
  gp_Pnt aP(13.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "PointOnSurface");
}

TEST_F(ExtremaPS_TorusComparisonTest, PointOnAxis)
{
  gp_Pnt aP(0.0, 0.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "PointOnAxis");
}

TEST_F(ExtremaPS_TorusComparisonTest, PointDiagonal)
{
  gp_Pnt aP(12.0, 12.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "PointDiagonal");
}

//==================================================================================================
// Different Torus Sizes
//==================================================================================================

TEST_F(ExtremaPS_TorusComparisonTest, LargeTorus_PointOutside)
{
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0, 10.0);
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(aTorus);
  GeomAdaptor_Surface          anAdaptor(aSurf);

  gp_Pnt aP(80.0, 0.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "LargeTorus_PointOutside");
}

TEST_F(ExtremaPS_TorusComparisonTest, ThinTorus_PointNear)
{
  // Major radius 10, minor radius 1 (thin ring)
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 1.0);
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(aTorus);
  GeomAdaptor_Surface          anAdaptor(aSurf);

  gp_Pnt aP(12.0, 0.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "ThinTorus_PointNear");
}

TEST_F(ExtremaPS_TorusComparisonTest, ThickTorus_PointInside)
{
  // Major radius 10, minor radius 8 (almost sphere-like, thick tube)
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 10.0, 8.0);
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(aTorus);
  GeomAdaptor_Surface          anAdaptor(aSurf);

  gp_Pnt aP(5.0, 0.0, 3.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "ThickTorus_PointInside");
}

//==================================================================================================
// Tilted Torus
//==================================================================================================

TEST_F(ExtremaPS_TorusComparisonTest, TiltedTorus_XAxis)
{
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 10.0, 3.0);
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(aTorus);
  GeomAdaptor_Surface          anAdaptor(aSurf);

  gp_Pnt aP(5.0, 15.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "TiltedTorus_XAxis");
}

TEST_F(ExtremaPS_TorusComparisonTest, TiltedTorus_Diagonal)
{
  gp_Dir aDir(1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0), 0.0);
  gp_Torus aTorus(gp_Ax3(gp_Pnt(0, 0, 0), aDir), 10.0, 3.0);
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(aTorus);
  GeomAdaptor_Surface          anAdaptor(aSurf);

  gp_Pnt aP(10.0, 10.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "TiltedTorus_Diagonal");
}

//==================================================================================================
// Translated Torus
//==================================================================================================

TEST_F(ExtremaPS_TorusComparisonTest, TranslatedTorus_PointNear)
{
  gp_Torus aTorus(gp_Ax3(gp_Pnt(50, 50, 20), gp_Dir(0, 0, 1)), 10.0, 3.0);
  Handle(Geom_ToroidalSurface) aSurf = new Geom_ToroidalSurface(aTorus);
  GeomAdaptor_Surface          anAdaptor(aSurf);

  gp_Pnt aP(60.0, 50.0, 20.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI, "TranslatedTorus_PointNear");
}

//==================================================================================================
// Bounded Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_TorusComparisonTest, PartialU_PointInRange)
{
  gp_Pnt aP(15.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, M_PI, 0.0, 2 * M_PI, "PartialU_PointInRange");
}

TEST_F(ExtremaPS_TorusComparisonTest, PartialV_PointInRange)
{
  gp_Pnt aP(15.0, 0.0, 2.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, M_PI, "PartialV_PointInRange");
}

TEST_F(ExtremaPS_TorusComparisonTest, SmallPatch)
{
  gp_Pnt aP(12.0, 2.0, 1.0);
  CompareMinDistances(aP, myAdaptor, 0.0, M_PI / 2, 0.0, M_PI / 2, "SmallPatch");
}

//==================================================================================================
// Stress Test
//==================================================================================================

TEST_F(ExtremaPS_TorusComparisonTest, StressTest_ToroidalPoints)
{
  int aPassCount  = 0;
  int aTotalCount = 0;

  // Test points at various positions around the torus
  for (int thetaU = 0; thetaU < 360; thetaU += 45)
  {
    double aU = thetaU * M_PI / 180.0;
    for (int thetaV = 0; thetaV < 360; thetaV += 60)
    {
      double aV = thetaV * M_PI / 180.0;
      for (double rScale = 0.5; rScale <= 2.0; rScale += 0.5)
      {
        // Point at varying distances from torus surface
        double aR = 10.0 + 3.0 * rScale * std::cos(aV);
        double x  = aR * std::cos(aU);
        double y  = aR * std::sin(aU);
        double z  = 3.0 * rScale * std::sin(aV);
        gp_Pnt aP(x, y, z);

        Extrema_ExtPS anOldExtPS(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 2 * M_PI,
                                 THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface aNewExtPS(myAdaptor, ExtremaPS::Domain2D(0.0, 2 * M_PI, 0.0, 2 * M_PI));
        const ExtremaPS::Result& aNewResult =aNewExtPS.PerformWithBoundary(aP, THE_TOLERANCE);

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

  EXPECT_EQ(aPassCount, aTotalCount);
}

