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

#include <BSplCLib.hxx>
#include <BSplCLib_Cache.hxx>
#include <BSplCLib_CacheGrid.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Vec.hxx>
#include <gp_Vec2d.hxx>

#include <NCollection_Array1.hxx>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_TOLERANCE = 1e-10;
}

//==================================================================================================
// Test fixture
//==================================================================================================

class BSplCLib_CacheGridTest : public ::testing::Test
{
protected:
  //! Creates flat knots array from knots and multiplicities
  void createFlatKnots(const NCollection_Array1<double>& theKnots,
                       const NCollection_Array1<int>&    theMults,
                       NCollection_Array1<double>&       theFlatKnots) const
  {
    int aFlatIndex = theFlatKnots.Lower();
    for (int i = theKnots.Lower(); i <= theKnots.Upper(); ++i)
    {
      for (int j = 0; j < theMults(i); ++j)
      {
        theFlatKnots(aFlatIndex++) = theKnots(i);
      }
    }
  }

  //! Setup a multi-span cubic B-spline with 3 spans: [0,1], [1,2], [2,3]
  void setupMultiSpan3D(NCollection_Array1<gp_Pnt>& thePoles,
                        NCollection_Array1<double>& theKnots,
                        NCollection_Array1<int>&    theMults,
                        NCollection_Array1<double>& theFlatKnots,
                        int&                        theDegree) const
  {
    theDegree = 3;
    // 6 poles for a cubic with 3 interior spans (knots 0, 1, 2, 3)
    thePoles.Resize(1, 6, false);
    thePoles(1) = gp_Pnt(0, 0, 0);
    thePoles(2) = gp_Pnt(0.5, 1, 0);
    thePoles(3) = gp_Pnt(1.5, 1.5, 0);
    thePoles(4) = gp_Pnt(2.0, 0.5, 1);
    thePoles(5) = gp_Pnt(2.5, -0.5, 0.5);
    thePoles(6) = gp_Pnt(3, 0, 0);

    theKnots.Resize(1, 4, false);
    theKnots(1) = 0.0;
    theKnots(2) = 1.0;
    theKnots(3) = 2.0;
    theKnots(4) = 3.0;

    theMults.Resize(1, 4, false);
    theMults(1) = 4; // degree+1 at ends
    theMults(2) = 1;
    theMults(3) = 1;
    theMults(4) = 4;

    // Total flat knots = 4 + 1 + 1 + 4 = 10
    theFlatKnots.Resize(1, 10, false);
    createFlatKnots(theKnots, theMults, theFlatKnots);
  }

  //! Setup a multi-span quadratic 2D B-spline
  void setupMultiSpan2D(NCollection_Array1<gp_Pnt2d>& thePoles,
                        NCollection_Array1<double>&   theFlatKnots,
                        int&                          theDegree) const
  {
    theDegree = 2;
    thePoles.Resize(1, 5, false);
    thePoles(1) = gp_Pnt2d(0, 0);
    thePoles(2) = gp_Pnt2d(1, 2);
    thePoles(3) = gp_Pnt2d(2, 1);
    thePoles(4) = gp_Pnt2d(3, 3);
    thePoles(5) = gp_Pnt2d(4, 0);

    NCollection_Array1<double> aKnots(1, 4);
    aKnots(1) = 0.0;
    aKnots(2) = 1.0;
    aKnots(3) = 2.0;
    aKnots(4) = 3.0;

    NCollection_Array1<int> aMults(1, 4);
    aMults(1) = 3; // degree+1
    aMults(2) = 1;
    aMults(3) = 1;
    aMults(4) = 3;

    // Total = 3 + 1 + 1 + 3 = 8
    theFlatKnots.Resize(1, 8, false);
    createFlatKnots(aKnots, aMults, theFlatKnots);
  }
};

//==================================================================================================
// D0 correctness - compare grid vs single cache
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, D0_MatchesSingleCache_3D)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.1)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

    gp_Pnt aGridPnt, aSinglePnt;
    aGrid->D0(u, aGridPnt);
    aSingleCache->D0(u, aSinglePnt);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "at u=" << u;
    EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE) << "at u=" << u;
    EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE) << "at u=" << u;
  }
}

TEST_F(BSplCLib_CacheGridTest, D0_MatchesSingleCache_2D)
{
  NCollection_Array1<gp_Pnt2d> aPoles;
  NCollection_Array1<double>   aFlatKnots;
  int                          aDeg = 0;
  setupMultiSpan2D(aPoles, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.1)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

    gp_Pnt2d aGridPnt, aSinglePnt;
    aGrid->D0(u, aGridPnt);
    aSingleCache->D0(u, aSinglePnt);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "at u=" << u;
    EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE) << "at u=" << u;
  }
}

//==================================================================================================
// D1 correctness
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, D1_MatchesSingleCache_3D)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.15)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

    gp_Pnt aGridPnt, aSinglePnt;
    gp_Vec aGridTan, aSingleTan;
    aGrid->D1(u, aGridPnt, aGridTan);
    aSingleCache->D1(u, aSinglePnt, aSingleTan);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "D1 point at u=" << u;
    EXPECT_NEAR(aGridTan.X(), aSingleTan.X(), THE_TOLERANCE) << "D1 tangent at u=" << u;
    EXPECT_NEAR(aGridTan.Y(), aSingleTan.Y(), THE_TOLERANCE) << "D1 tangent at u=" << u;
    EXPECT_NEAR(aGridTan.Z(), aSingleTan.Z(), THE_TOLERANCE) << "D1 tangent at u=" << u;
  }
}

//==================================================================================================
// D2 correctness
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, D2_MatchesSingleCache_3D)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.2)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

    gp_Pnt aGridPnt, aSinglePnt;
    gp_Vec aGridD1, aSingleD1, aGridD2, aSingleD2;
    aGrid->D2(u, aGridPnt, aGridD1, aGridD2);
    aSingleCache->D2(u, aSinglePnt, aSingleD1, aSingleD2);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "D2 point at u=" << u;
    EXPECT_NEAR(aGridD2.X(), aSingleD2.X(), THE_TOLERANCE) << "D2 curvature at u=" << u;
    EXPECT_NEAR(aGridD2.Y(), aSingleD2.Y(), THE_TOLERANCE) << "D2 curvature at u=" << u;
    EXPECT_NEAR(aGridD2.Z(), aSingleD2.Z(), THE_TOLERANCE) << "D2 curvature at u=" << u;
  }
}

//==================================================================================================
// D3 correctness
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, D3_MatchesSingleCache_3D)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.25)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

    gp_Pnt aGridPnt, aSinglePnt;
    gp_Vec aGridD1, aSingleD1, aGridD2, aSingleD2, aGridD3, aSingleD3;
    aGrid->D3(u, aGridPnt, aGridD1, aGridD2, aGridD3);
    aSingleCache->D3(u, aSinglePnt, aSingleD1, aSingleD2, aSingleD3);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "D3 point at u=" << u;
    EXPECT_NEAR(aGridD3.X(), aSingleD3.X(), THE_TOLERANCE) << "D3 torsion at u=" << u;
    EXPECT_NEAR(aGridD3.Y(), aSingleD3.Y(), THE_TOLERANCE) << "D3 torsion at u=" << u;
    EXPECT_NEAR(aGridD3.Z(), aSingleD3.Z(), THE_TOLERANCE) << "D3 torsion at u=" << u;
  }
}

//==================================================================================================
// Grid shift / span boundary crossing test
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, SpanBoundary_Oscillation)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  // First evaluate at center to set up the grid around span boundary 1.0
  gp_Pnt aDummyPnt;
  aGrid->D0(1.5, aDummyPnt);

  // Oscillate near span boundary at u=1.0
  const double aParams[] = {0.95, 1.05, 0.98, 1.02, 0.99, 1.01, 1.0};
  for (double u : aParams)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

    gp_Pnt aGridPnt, aSinglePnt;
    aGrid->D0(u, aGridPnt);
    aSingleCache->D0(u, aSinglePnt);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "Oscillation at u=" << u;
    EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE) << "Oscillation at u=" << u;
    EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE) << "Oscillation at u=" << u;
  }
}

//==================================================================================================
// Single-span curve (Bezier-like BSpline)
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, SingleSpan_D0)
{
  // Cubic Bezier has 1 span
  NCollection_Array1<gp_Pnt> aPoles(1, 4);
  aPoles(1) = gp_Pnt(0, 0, 0);
  aPoles(2) = gp_Pnt(1, 2, 0);
  aPoles(3) = gp_Pnt(2, 2, 0);
  aPoles(4) = gp_Pnt(3, 0, 0);

  NCollection_Array1<double> aFlatKnots(1, 8);
  for (int i = 1; i <= 4; ++i)
    aFlatKnots(i) = 0.0;
  for (int i = 5; i <= 8; ++i)
    aFlatKnots(i) = 1.0;

  const int aDeg = 3;

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);
  aSingleCache->BuildCache(0.5, aFlatKnots, aPoles, nullptr);

  for (double u = 0.0; u <= 1.0; u += 0.1)
  {
    gp_Pnt aGridPnt, aSinglePnt;
    aGrid->D0(u, aGridPnt);
    aSingleCache->D0(u, aSinglePnt);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE);
    EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE);
    EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE);
  }
}

//==================================================================================================
// Grid shift when parameter leaves all cells
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, GridShift_LargeJump)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, nullptr);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, nullptr);

  // Build at start (center span = [0,1])
  // Jump to far end, requiring full grid rebuild
  const double u = 2.8;
  aSingleCache->BuildCache(u, aFlatKnots, aPoles, nullptr);

  gp_Pnt aGridPnt, aSinglePnt;
  aGrid->D0(u, aGridPnt);
  aSingleCache->D0(u, aSinglePnt);

  EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE);
  EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE);
  EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE);
}

//==================================================================================================
// Rational (weighted) curve test
//==================================================================================================

TEST_F(BSplCLib_CacheGridTest, D0_RationalCurve_MatchesSingleCache)
{
  NCollection_Array1<gp_Pnt> aPoles;
  NCollection_Array1<double> aKnots, aFlatKnots;
  NCollection_Array1<int>    aMults;
  int                        aDeg = 0;
  setupMultiSpan3D(aPoles, aKnots, aMults, aFlatKnots, aDeg);

  NCollection_Array1<double> aWeights(1, 6);
  aWeights(1) = 1.0;
  aWeights(2) = 2.0;
  aWeights(3) = 1.5;
  aWeights(4) = 0.5;
  aWeights(5) = 1.0;
  aWeights(6) = 1.0;

  occ::handle<BSplCLib_CacheGrid> aGrid =
    new BSplCLib_CacheGrid(aDeg, false, aFlatKnots, aPoles, &aWeights);
  occ::handle<BSplCLib_Cache> aSingleCache =
    new BSplCLib_Cache(aDeg, false, aFlatKnots, aPoles, &aWeights);

  for (double u = 0.0; u <= 3.0; u += 0.15)
  {
    if (!aSingleCache->IsCacheValid(u))
      aSingleCache->BuildCache(u, aFlatKnots, aPoles, &aWeights);

    gp_Pnt aGridPnt, aSinglePnt;
    aGrid->D0(u, aGridPnt);
    aSingleCache->D0(u, aSinglePnt);

    EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "Rational D0 at u=" << u;
    EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE) << "Rational D0 at u=" << u;
    EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE) << "Rational D0 at u=" << u;
  }
}
