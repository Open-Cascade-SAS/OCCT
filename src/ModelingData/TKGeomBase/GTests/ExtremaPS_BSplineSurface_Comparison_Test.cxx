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

#include <Geom_BSplineSurface.hxx>
#include <GeomAdaptor_Surface.hxx>
#include <gp_Pnt.hxx>
#include <TColgp_Array2OfPnt.hxx>
#include <TColStd_Array1OfInteger.hxx>
#include <TColStd_Array1OfReal.hxx>

#include <cmath>

namespace
{
const double THE_TOLERANCE      = 1.0e-6;
const double THE_DIST_TOLERANCE = 0.03; // Relaxed for numerical surfaces (grid-based vs iterative)

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

  ASSERT_TRUE(anOldExtPS.IsDone()) << theTestName << ": Old implementation failed";
  ASSERT_EQ(aNewResult.Status, ExtremaPS::Status::OK) << theTestName << ": New implementation failed";

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

//! Create a flat BSpline surface
//! For 4 poles with degree 3: sum of multiplicities = 4 + 3 + 1 = 8
Handle(Geom_BSplineSurface) MakeFlatBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 4, 1, 4);
  for (int i = 1; i <= 4; ++i)
  {
    for (int j = 1; j <= 4; ++j)
    {
      aPoles(i, j) = gp_Pnt((i - 1) * 5.0, (j - 1) * 5.0, 0.0);
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 2), aVKnots(1, 2);
  TColStd_Array1OfInteger aUMults(1, 2), aVMults(1, 2);

  aUKnots(1) = 0.0;
  aUKnots(2) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 1.0;

  // For 4 poles, deg 3: need 4+3+1 = 8 total multiplicity
  aUMults(1) = 4;
  aUMults(2) = 4;  // 4+4 = 8 ✓
  aVMults(1) = 4;
  aVMults(2) = 4;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
}

//! Create a dome-like BSpline surface
//! For 5 poles with degree 2: sum of multiplicities = 5 + 2 + 1 = 8
Handle(Geom_BSplineSurface) MakeDomeBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double x = (i - 1) * 2.5;
      double y = (j - 1) * 2.5;
      // Create dome shape: center raised
      double cx = 5.0, cy = 5.0;
      double r  = std::sqrt((x - cx) * (x - cx) + (y - cy) * (y - cy));
      double z  = std::max(0.0, 5.0 - r * 0.5);
      aPoles(i, j) = gp_Pnt(x, y, z);
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 3), aVKnots(1, 3);
  TColStd_Array1OfInteger aUMults(1, 3), aVMults(1, 3);

  aUKnots(1) = 0.0;
  aUKnots(2) = 0.5;
  aUKnots(3) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.5;
  aVKnots(3) = 1.0;

  // For 5 poles, deg 2: need 5+2+1 = 8 total multiplicity
  aUMults(1) = 3;  // degree + 1 at start
  aUMults(2) = 2;  // 8 - 3 - 3 = 2
  aUMults(3) = 3;  // degree + 1 at end  -> 3+2+3 = 8 ✓
  aVMults(1) = 3;
  aVMults(2) = 2;
  aVMults(3) = 3;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

//! Create a saddle-like BSpline surface
//! For 5 poles with degree 2: sum of multiplicities = 5 + 2 + 1 = 8
Handle(Geom_BSplineSurface) MakeSaddleBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 5, 1, 5);
  for (int i = 1; i <= 5; ++i)
  {
    for (int j = 1; j <= 5; ++j)
    {
      double x = (i - 1) * 2.5;
      double y = (j - 1) * 2.5;
      // Create saddle shape: z = (x-5)^2 - (y-5)^2
      double z = ((x - 5.0) * (x - 5.0) - (y - 5.0) * (y - 5.0)) * 0.1;
      aPoles(i, j) = gp_Pnt(x, y, z);
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 3), aVKnots(1, 3);
  TColStd_Array1OfInteger aUMults(1, 3), aVMults(1, 3);

  aUKnots(1) = 0.0;
  aUKnots(2) = 0.5;
  aUKnots(3) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.5;
  aVKnots(3) = 1.0;

  // For 5 poles, deg 2: need 8 total multiplicity
  aUMults(1) = 3;
  aUMults(2) = 2;
  aUMults(3) = 3;  // 3+2+3 = 8 ✓
  aVMults(1) = 3;
  aVMults(2) = 2;
  aVMults(3) = 3;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 2, 2);
}

//! Create a wavy BSpline surface with more control points
//! For 7 poles with degree 3: sum of multiplicities = 7 + 3 + 1 = 11
Handle(Geom_BSplineSurface) MakeWavyBSpline()
{
  TColgp_Array2OfPnt aPoles(1, 7, 1, 7);
  for (int i = 1; i <= 7; ++i)
  {
    for (int j = 1; j <= 7; ++j)
    {
      double x = (i - 1) * 10.0 / 6.0;
      double y = (j - 1) * 10.0 / 6.0;
      double z = 2.0 * std::sin(x * 0.8) * std::cos(y * 0.8);
      aPoles(i, j) = gp_Pnt(x, y, z);
    }
  }

  TColStd_Array1OfReal    aUKnots(1, 4), aVKnots(1, 4);
  TColStd_Array1OfInteger aUMults(1, 4), aVMults(1, 4);

  aUKnots(1) = 0.0;
  aUKnots(2) = 0.33;
  aUKnots(3) = 0.66;
  aUKnots(4) = 1.0;
  aVKnots(1) = 0.0;
  aVKnots(2) = 0.33;
  aVKnots(3) = 0.66;
  aVKnots(4) = 1.0;

  // For 7 poles, deg 3: need 7+3+1 = 11 total multiplicity
  aUMults(1) = 4;  // degree + 1 at start
  aUMults(2) = 2;
  aUMults(3) = 1;
  aUMults(4) = 4;  // degree + 1 at end  -> 4+2+1+4 = 11 ✓
  aVMults(1) = 4;
  aVMults(2) = 2;
  aVMults(3) = 1;
  aVMults(4) = 4;

  return new Geom_BSplineSurface(aPoles, aUKnots, aVKnots, aUMults, aVMults, 3, 3);
}
} // namespace

//==================================================================================================
// Test fixture
//==================================================================================================
class ExtremaPS_BSplineSurfaceComparisonTest : public testing::Test
{
protected:
  void SetUp() override
  {
    myFlatSurface   = MakeFlatBSpline();
    myDomeSurface   = MakeDomeBSpline();
    mySaddleSurface = MakeSaddleBSpline();
    myWavySurface   = MakeWavyBSpline();
  }

  Handle(Geom_BSplineSurface) myFlatSurface;
  Handle(Geom_BSplineSurface) myDomeSurface;
  Handle(Geom_BSplineSurface) mySaddleSurface;
  Handle(Geom_BSplineSurface) myWavySurface;
};

//==================================================================================================
// Flat Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, FlatSurface_PointAbove)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  gp_Pnt              aP(7.5, 7.5, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "FlatSurface_PointAbove");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, FlatSurface_PointOnSurface)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  gp_Pnt              aP(7.5, 7.5, 0.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "FlatSurface_PointOnSurface");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, FlatSurface_PointAtCorner)
{
  GeomAdaptor_Surface anAdaptor(myFlatSurface);
  gp_Pnt              aP(-3.0, -3.0, 5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "FlatSurface_PointAtCorner");
}

//==================================================================================================
// Dome Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, DomeSurface_PointAbovePeak)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(5.0, 5.0, 15.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "DomeSurface_PointAbovePeak");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, DomeSurface_PointBelowPeak)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(5.0, 5.0, -5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "DomeSurface_PointBelowPeak");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, DomeSurface_PointToSide)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(15.0, 5.0, 5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "DomeSurface_PointToSide");
}

//==================================================================================================
// Saddle Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, SaddleSurface_PointAboveCenter)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  gp_Pnt              aP(5.0, 5.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "SaddleSurface_PointAboveCenter");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, SaddleSurface_PointNearCorner)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  gp_Pnt              aP(0.0, 0.0, 5.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "SaddleSurface_PointNearCorner");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, SaddleSurface_PointBelowSaddle)
{
  GeomAdaptor_Surface anAdaptor(mySaddleSurface);
  gp_Pnt              aP(5.0, 5.0, -10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "SaddleSurface_PointBelowSaddle");
}

//==================================================================================================
// Wavy Surface Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, WavySurface_PointAbove)
{
  GeomAdaptor_Surface anAdaptor(myWavySurface);
  gp_Pnt              aP(5.0, 5.0, 10.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "WavySurface_PointAbove");
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, WavySurface_PointNear)
{
  GeomAdaptor_Surface anAdaptor(myWavySurface);
  gp_Pnt              aP(3.0, 3.0, 3.0);
  CompareMinDistances(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0, "WavySurface_PointNear");
}

//==================================================================================================
// Bounded Domain Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, DomeSurface_PartialDomain)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  gp_Pnt              aP(7.0, 7.0, 8.0);
  CompareMinDistances(aP, anAdaptor, 0.3, 0.7, 0.3, 0.7, "DomeSurface_PartialDomain");
}

//==================================================================================================
// Stress Tests
//==================================================================================================

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, StressTest_DomeSurface)
{
  GeomAdaptor_Surface anAdaptor(myDomeSurface);
  int                 aPassCount  = 0;
  int                 aTotalCount = 0;

  for (double x = -5.0; x <= 15.0; x += 5.0)
  {
    for (double y = -5.0; y <= 15.0; y += 5.0)
    {
      for (double z = -5.0; z <= 15.0; z += 5.0)
      {
        gp_Pnt aP(x, y, z);

        Extrema_ExtPS anOldExtPS(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0,
                                 THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface aNewExtPS(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
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

          // Pass if results match within tolerance OR new finds closer point
          double aRelDiff = std::abs(std::sqrt(aOldMin) - std::sqrt(aNewMin)) /
                            std::max(std::sqrt(aOldMin), 1.0);
          if (aRelDiff < 0.05 || aNewMin < aOldMin)
          {
            ++aPassCount;
          }
        }
      }
    }
  }

  // New algorithm must match or find better (closer) results than old algorithm.
  EXPECT_EQ(aPassCount, aTotalCount) << "All cases should match or improve";
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, StressTest_SaddleSurface)
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

        Extrema_ExtPS anOldExtPS(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0,
                                 THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface aNewExtPS(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
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

          // Pass if results match within tolerance OR new finds closer point
          double aRelDiff = std::abs(std::sqrt(aOldMin) - std::sqrt(aNewMin)) /
                            std::max(std::sqrt(aOldMin), 1.0);
          if (aRelDiff < 0.05 || aNewMin < aOldMin)
          {
            ++aPassCount;
          }
        }
      }
    }
  }

  // New algorithm must match or find better (closer) results than old algorithm.
  EXPECT_EQ(aPassCount, aTotalCount) << "All cases should match or improve";
}

TEST_F(ExtremaPS_BSplineSurfaceComparisonTest, StressTest_WavySurface)
{
  GeomAdaptor_Surface anAdaptor(myWavySurface);
  int                 aPassCount  = 0;
  int                 aTotalCount = 0;

  for (double x = -3.0; x <= 13.0; x += 4.0)
  {
    for (double y = -3.0; y <= 13.0; y += 4.0)
    {
      for (double z = -5.0; z <= 10.0; z += 3.0)
      {
        gp_Pnt aP(x, y, z);

        Extrema_ExtPS anOldExtPS(aP, anAdaptor, 0.0, 1.0, 0.0, 1.0,
                                 THE_TOLERANCE, THE_TOLERANCE);

        ExtremaPS_Surface aNewExtPS(anAdaptor, ExtremaPS::Domain2D(0.0, 1.0, 0.0, 1.0));
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

          // Pass if results match within tolerance OR new finds closer point
          double aRelDiff = std::abs(std::sqrt(aOldMin) - std::sqrt(aNewMin)) /
                            std::max(std::sqrt(aOldMin), 1.0);
          if (aRelDiff < 0.10 || aNewMin < aOldMin)
          {
            ++aPassCount;
          }
        }
      }
    }
  }

  // New algorithm must match or find better (closer) results than old algorithm.
  EXPECT_EQ(aPassCount, aTotalCount) << "All cases should match or improve";
}

