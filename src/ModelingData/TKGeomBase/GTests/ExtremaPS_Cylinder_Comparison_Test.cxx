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

#include <Geom_CylindricalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>

#include <cmath>
#include <vector>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 1.0e-3; // Relaxed for cylinder extrema

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

  // Handle infinite solutions case (e.g., point on cylinder axis)
  if (aNewResult.Status == ExtremaPS::Status::InfiniteSolutions)
  {
    // This is valid - just verify the infinite distance is reasonable
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
class ExtremaPS_CylinderComparisonTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Cylinder along Z axis, radius 5
    gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
    myCylinder = new Geom_CylindricalSurface(aCyl);
    myAdaptor.Load(myCylinder);
  }

  Handle(Geom_CylindricalSurface) myCylinder;
  GeomAdaptor_Surface             myAdaptor;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderComparisonTest, PointOutsideCylinder)
{
  gp_Pnt aP(10.0, 0.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "PointOutsideCylinder");
}

TEST_F(ExtremaPS_CylinderComparisonTest, PointInsideCylinder)
{
  gp_Pnt aP(2.0, 0.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "PointInsideCylinder");
}

TEST_F(ExtremaPS_CylinderComparisonTest, PointOnAxis)
{
  gp_Pnt aP(0.0, 0.0, 10.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "PointOnAxis");
}

TEST_F(ExtremaPS_CylinderComparisonTest, PointOnSurface)
{
  gp_Pnt aP(5.0, 0.0, 3.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "PointOnSurface");
}

TEST_F(ExtremaPS_CylinderComparisonTest, PointDiagonal)
{
  gp_Pnt aP(7.0, 7.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "PointDiagonal");
}

//==================================================================================================
// Different Radii
//==================================================================================================

TEST_F(ExtremaPS_CylinderComparisonTest, LargeRadius_PointOutside)
{
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0);
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(aCyl);
  GeomAdaptor_Surface anAdaptor(aSurf);

  gp_Pnt aP(100.0, 0.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -100.0, 100.0, "LargeRadius_PointOutside");
}

TEST_F(ExtremaPS_CylinderComparisonTest, SmallRadius_PointNear)
{
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.5);
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(aCyl);
  GeomAdaptor_Surface anAdaptor(aSurf);

  gp_Pnt aP(1.0, 0.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -10.0, 10.0, "SmallRadius_PointNear");
}

//==================================================================================================
// Tilted Cylinder
//==================================================================================================

TEST_F(ExtremaPS_CylinderComparisonTest, TiltedCylinder_XAxis)
{
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(1, 0, 0)), 5.0);
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(aCyl);
  GeomAdaptor_Surface anAdaptor(aSurf);

  gp_Pnt aP(10.0, 10.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "TiltedCylinder_XAxis");
}

TEST_F(ExtremaPS_CylinderComparisonTest, TiltedCylinder_Diagonal)
{
  gp_Dir aDir(1.0 / std::sqrt(2.0), 1.0 / std::sqrt(2.0), 0.0);
  gp_Cylinder aCyl(gp_Ax3(gp_Pnt(0, 0, 0), aDir), 5.0);
  Handle(Geom_CylindricalSurface) aSurf = new Geom_CylindricalSurface(aCyl);
  GeomAdaptor_Surface anAdaptor(aSurf);

  gp_Pnt aP(10.0, 10.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -50.0, 50.0, "TiltedCylinder_Diagonal");
}

//==================================================================================================
// Bounded Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_CylinderComparisonTest, PartialU_PointInRange)
{
  gp_Pnt aP(10.0, 0.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, M_PI, -50.0, 50.0, "PartialU_PointInRange");
}

TEST_F(ExtremaPS_CylinderComparisonTest, PartialV_PointInRange)
{
  gp_Pnt aP(10.0, 0.0, 5.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, 10.0, "PartialV_PointInRange");
}

TEST_F(ExtremaPS_CylinderComparisonTest, SmallPatch)
{
  gp_Pnt aP(6.0, 1.0, 2.0);
  CompareMinDistances(aP, myAdaptor, 0.0, M_PI / 2, 0.0, 5.0, "SmallPatch");
}

//==================================================================================================
// Multiple Points Stress Test
//==================================================================================================

TEST_F(ExtremaPS_CylinderComparisonTest, StressTest_CircularPoints)
{
  int aPassCount  = 0;
  int aTotalCount = 0;

  // Test points around and along the cylinder
  for (int angle = 0; angle < 360; angle += 30)
  {
    double aRad = angle * M_PI / 180.0;
    for (double r = 2.0; r <= 10.0; r += 2.0)
    {
      for (double z = -10.0; z <= 10.0; z += 5.0)
      {
        gp_Pnt aP(r * std::cos(aRad), r * std::sin(aRad), z);

        Extrema_ExtPS anOldExtPS(aP, myAdaptor, 0.0, 2 * M_PI, -50.0, 50.0,
                                 THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface aNewExtPS(myAdaptor, ExtremaPS::Domain2D(0.0, 2 * M_PI, -50.0, 50.0));
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
