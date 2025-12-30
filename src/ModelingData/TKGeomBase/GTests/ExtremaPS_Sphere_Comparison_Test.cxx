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

#include <Geom_SphericalSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Ax3.hxx>
#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Sphere.hxx>

#include <cmath>
#include <vector>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 1.0e-3; // Relaxed for sphere extrema

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

  // New implementation must always succeed
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
class ExtremaPS_SphereComparisonTest : public testing::Test
{
protected:
  void SetUp() override
  {
    // Sphere at origin with radius 5
    gp_Sphere aSph(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 5.0);
    mySphere = new Geom_SphericalSurface(aSph);
    myAdaptor.Load(mySphere);
  }

  occ::handle<Geom_SphericalSurface> mySphere;
  GeomAdaptor_Surface           myAdaptor;
};

//==================================================================================================
// Basic Tests
//==================================================================================================

TEST_F(ExtremaPS_SphereComparisonTest, PointOutsideSphere)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "PointOutsideSphere");
}

TEST_F(ExtremaPS_SphereComparisonTest, PointInsideSphere)
{
  gp_Pnt aP(2.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "PointInsideSphere");
}

TEST_F(ExtremaPS_SphereComparisonTest, PointOnSurface)
{
  gp_Pnt aP(5.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "PointOnSurface");
}

TEST_F(ExtremaPS_SphereComparisonTest, PointAtNorthPole)
{
  gp_Pnt aP(0.0, 0.0, 10.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "PointAtNorthPole");
}

TEST_F(ExtremaPS_SphereComparisonTest, PointAtSouthPole)
{
  gp_Pnt aP(0.0, 0.0, -10.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "PointAtSouthPole");
}

TEST_F(ExtremaPS_SphereComparisonTest, PointDiagonal)
{
  gp_Pnt aP(7.0, 7.0, 7.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "PointDiagonal");
}

//==================================================================================================
// Different Radii
//==================================================================================================

TEST_F(ExtremaPS_SphereComparisonTest, LargeRadius_PointOutside)
{
  gp_Sphere aSph(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 50.0);
  occ::handle<Geom_SphericalSurface> aSurf = new Geom_SphericalSurface(aSph);
  GeomAdaptor_Surface           anAdaptor(aSurf);

  gp_Pnt aP(100.0, 0.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "LargeRadius_PointOutside");
}

TEST_F(ExtremaPS_SphereComparisonTest, SmallRadius_PointNear)
{
  gp_Sphere aSph(gp_Ax3(gp_Pnt(0, 0, 0), gp_Dir(0, 0, 1)), 0.5);
  occ::handle<Geom_SphericalSurface> aSurf = new Geom_SphericalSurface(aSph);
  GeomAdaptor_Surface           anAdaptor(aSurf);

  gp_Pnt aP(1.0, 0.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "SmallRadius_PointNear");
}

//==================================================================================================
// Translated Sphere
//==================================================================================================

TEST_F(ExtremaPS_SphereComparisonTest, TranslatedSphere_PointNear)
{
  gp_Sphere aSph(gp_Ax3(gp_Pnt(10, 20, 30), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_SphericalSurface> aSurf = new Geom_SphericalSurface(aSph);
  GeomAdaptor_Surface           anAdaptor(aSurf);

  gp_Pnt aP(20.0, 20.0, 30.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "TranslatedSphere_PointNear");
}

TEST_F(ExtremaPS_SphereComparisonTest, TranslatedSphere_PointInside)
{
  gp_Sphere aSph(gp_Ax3(gp_Pnt(10, 20, 30), gp_Dir(0, 0, 1)), 5.0);
  occ::handle<Geom_SphericalSurface> aSurf = new Geom_SphericalSurface(aSph);
  GeomAdaptor_Surface           anAdaptor(aSurf);

  gp_Pnt aP(12.0, 21.0, 31.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2, "TranslatedSphere_PointInside");
}

//==================================================================================================
// Bounded Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_SphereComparisonTest, PartialU_PointInRange)
{
  gp_Pnt aP(10.0, 0.0, 0.0);
  CompareMinDistances(aP, myAdaptor, 0.0, M_PI, -M_PI / 2, M_PI / 2, "PartialU_PointInRange");
}

TEST_F(ExtremaPS_SphereComparisonTest, PartialV_PointInRange)
{
  gp_Pnt aP(10.0, 0.0, 3.0);
  CompareMinDistances(aP, myAdaptor, 0.0, 2 * M_PI, 0.0, M_PI / 2, "PartialV_PointInRange");
}

TEST_F(ExtremaPS_SphereComparisonTest, SmallPatch)
{
  gp_Pnt aP(6.0, 1.0, 2.0);
  CompareMinDistances(aP, myAdaptor, 0.0, M_PI / 2, 0.0, M_PI / 4, "SmallPatch");
}

//==================================================================================================
// Stress Test
//==================================================================================================

TEST_F(ExtremaPS_SphereComparisonTest, StressTest_SphericalPoints)
{
  int aPassCount  = 0;
  int aTotalCount = 0;

  // Test points at various positions around the sphere
  for (int theta = 0; theta < 360; theta += 45)
  {
    double aThetaRad = theta * M_PI / 180.0;
    for (int phi = -80; phi <= 80; phi += 40)
    {
      double aPhiRad = phi * M_PI / 180.0;
      for (double r = 2.0; r <= 10.0; r += 2.0)
      {
        double x = r * std::cos(aPhiRad) * std::cos(aThetaRad);
        double y = r * std::cos(aPhiRad) * std::sin(aThetaRad);
        double z = r * std::sin(aPhiRad);
        gp_Pnt aP(x, y, z);

        Extrema_ExtPS anOldExtPS(aP, myAdaptor, 0.0, 2 * M_PI, -M_PI / 2, M_PI / 2,
                                 THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface aNewExtPS(myAdaptor, ExtremaPS::Domain2D(0.0, 2 * M_PI, -M_PI / 2, M_PI / 2));
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

