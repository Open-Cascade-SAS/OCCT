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

#include <Geom_BezierSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 0.01; // Relaxed for numerical surfaces (grid-based vs iterative)

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

  ASSERT_TRUE(anOldExtPS.IsDone()) << theTestName << ": Old implementation failed";
  ASSERT_EQ(aNewResult.Status, ExtremaPS::Status::OK)
    << theTestName << ": New implementation failed";

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

//! Create a flat Bezier surface (planar-like)
occ::handle<Geom_BezierSurface> MakeFlatBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  for (int i = 1; i <= 3; ++i)
  {
    for (int j = 1; j <= 3; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) * 5.0, (j - 1) * 5.0, 0.0);
    }
  }
  return new Geom_BezierSurface(aPoles);
}

//! Create a dome-like Bezier surface
occ::handle<Geom_BezierSurface> MakeDomeBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  // Create a 3x3 grid with center raised
  aPoles(1, 1) = gp_Pnt(0, 0, 0);
  aPoles(1, 2) = gp_Pnt(0, 5, 2);
  aPoles(1, 3) = gp_Pnt(0, 10, 0);

  aPoles(2, 1) = gp_Pnt(5, 0, 2);
  aPoles(2, 2) = gp_Pnt(5, 5, 8); // Peak
  aPoles(2, 3) = gp_Pnt(5, 10, 2);

  aPoles(3, 1) = gp_Pnt(10, 0, 0);
  aPoles(3, 2) = gp_Pnt(10, 5, 2);
  aPoles(3, 3) = gp_Pnt(10, 10, 0);

  return new Geom_BezierSurface(aPoles);
}

//! Create a saddle-like Bezier surface
occ::handle<Geom_BezierSurface> MakeSaddleBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 3, 1, 3);
  // Create saddle: corners up, sides down, center at origin level
  aPoles(1, 1) = gp_Pnt(0, 0, 5);
  aPoles(1, 2) = gp_Pnt(0, 5, -2);
  aPoles(1, 3) = gp_Pnt(0, 10, 5);

  aPoles(2, 1) = gp_Pnt(5, 0, -2);
  aPoles(2, 2) = gp_Pnt(5, 5, 0);
  aPoles(2, 3) = gp_Pnt(5, 10, -2);

  aPoles(3, 1) = gp_Pnt(10, 0, 5);
  aPoles(3, 2) = gp_Pnt(10, 5, -2);
  aPoles(3, 3) = gp_Pnt(10, 10, 5);

  return new Geom_BezierSurface(aPoles);
}

//! Create a wavy Bezier surface (higher degree)
occ::handle<Geom_BezierSurface> MakeWavyBezier()
{
  NCollection_Array2<gp_Pnt> aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      double x     = (i - 1) * 10.0 / 3.0;
      double y     = (j - 1) * 10.0 / 3.0;
      double z     = 2.0 * std::sin(x * 0.5) * std::cos(y * 0.5);
      aPoles(i, j) = gp_Pnt(x, y, z);
    }
  }
  return new Geom_BezierSurface(aPoles);
}
} // namespace

//==================================================================================================
// Test fixture
//==================================================================================================
class ExtremaPS_BezierSurfaceComparisonTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myFlatSurface   = MakeFlatBezier();
    myDomeSurface   = MakeDomeBezier();
    mySaddleSurface = MakeSaddleBezier();
    myWavySurface   = MakeWavyBezier();
  }

  occ::handle<Geom_BezierSurface> myFlatSurface;
  occ::handle<Geom_BezierSurface> myDomeSurface;
  occ::handle<Geom_BezierSurface> mySaddleSurface;
  occ::handle<Geom_BezierSurface> myWavySurface;
};

//==================================================================================================
// Flat Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, FlatSurface_PointAbove)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  gp_Pnt              aP(5.0, 5.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "FlatSurface_PointAbove");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, FlatSurface_PointOnSurface)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  gp_Pnt              aP(5.0, 5.0, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "FlatSurface_PointOnSurface");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, FlatSurface_PointAtCorner)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  gp_Pnt              aP(-2.0, -2.0, 5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "FlatSurface_PointAtCorner");
}

//==================================================================================================
// Dome Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, DomeSurface_PointAbovePeak)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(5.0, 5.0, 15.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "DomeSurface_PointAbovePeak");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, DomeSurface_PointBelowPeak)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(5.0, 5.0, -5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "DomeSurface_PointBelowPeak");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, DomeSurface_PointToSide)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(15.0, 5.0, 5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "DomeSurface_PointToSide");
}

//==================================================================================================
// Saddle Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, SaddleSurface_PointAboveCenter)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  gp_Pnt              aP(5.0, 5.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "SaddleSurface_PointAboveCenter");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, SaddleSurface_PointNearCorner)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  gp_Pnt              aP(0.0, 0.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "SaddleSurface_PointNearCorner");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, SaddleSurface_PointBelowSaddle)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  gp_Pnt              aP(5.0, 5.0, -10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "SaddleSurface_PointBelowSaddle");
}

//==================================================================================================
// Wavy Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, WavySurface_PointAbove)
{
  GeomAdaptor_Surface anAdaptor(myWavySurface);
  gp_Pnt              aP(5.0, 5.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "WavySurface_PointAbove");
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, WavySurface_PointNear)
{
  GeomAdaptor_Surface anAdaptor(myWavySurface);
  gp_Pnt              aP(3.0, 3.0, 3.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "WavySurface_PointNear");
}

//==================================================================================================
// Bounded Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, DomeSurface_PartialDomain)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(7.0, 7.0, 8.0);
  CompareMinDistances(aP, anAdaptor, 0.3, 0.7, 0.3, 0.7, "DomeSurface_PartialDomain");
}

//==================================================================================================
// Stress Test
//==================================================================================================

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, StressTest_DomeSurface)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  int                 aPassCount  = 0;
  int                 aTotalCount = 0;

  // Test grid of points around the surface
  for (double x = -5.0; x <= 15.0; x += 5.0)
  {
    for (double y = -5.0; y <= 15.0; y += 5.0)
    {
      for (double z = -5.0; z <= 15.0; z += 5.0)
      {
        gp_Pnt aP(x, y, z);

        Extrema_ExtPS anOldExtPS(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface        aNewExtPS(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
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

          // Use relative tolerance (5%) for numerical surfaces
          double aRelDiff =
            std::abs(std::sqrt(aOldMin) - std::sqrt(aNewMin)) / std::max(std::sqrt(aOldMin), 1.0);
          if (aRelDiff < 0.05) // 5% relative tolerance
          {
            ++aPassCount;
          }
        }
      }
    }
  }

  // Allow some failures due to numerical differences
  EXPECT_GE(aPassCount, aTotalCount * 0.80) << "At least 80% should match";
}

TEST_F(ExtremaPS_BezierSurfaceComparisonTest, StressTest_SaddleSurface)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  int                 aPassCount  = 0;
  int                 aTotalCount = 0;

  for (double x = -5.0; x <= 15.0; x += 5.0)
  {
    for (double y = -5.0; y <= 15.0; y += 5.0)
    {
      for (double z = -10.0; z <= 15.0; z += 5.0)
      {
        gp_Pnt aP(x, y, z);

        Extrema_ExtPS anOldExtPS(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface        aNewExtPS(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
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

          // Use relative tolerance (5%) for numerical surfaces
          double aRelDiff =
            std::abs(std::sqrt(aOldMin) - std::sqrt(aNewMin)) / std::max(std::sqrt(aOldMin), 1.0);
          if (aRelDiff < 0.05)
          {
            ++aPassCount;
          }
        }
      }
    }
  }

  // Allow some failures due to numerical differences
  EXPECT_GE(aPassCount, aTotalCount * 0.80) << "At least 80% should match";
}
