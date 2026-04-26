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
#include <BSplSLib_Cache.hxx>
#include <BSplSLib_CacheGrid.hxx>

#include <gp_Pnt.hxx>
#include <gp_Vec.hxx>

#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

#include <gtest/gtest.h>

namespace
{
constexpr double THE_TOLERANCE = 1e-10;
}

//==================================================================================================
// Test fixture
//==================================================================================================

class BSplSLib_CacheGridTest : public ::testing::Test
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

  //! Setup a biquadratic B-spline surface with multiple spans in both U and V.
  //! U: 3 spans [0,1], [1,2], [2,3]
  //! V: 2 spans [0,1], [1,2]
  void setupMultiSpanSurface(NCollection_Array2<gp_Pnt>& thePoles,
                             NCollection_Array1<double>& theFlatKnotsU,
                             NCollection_Array1<double>& theFlatKnotsV,
                             int&                        theDegreeU,
                             int&                        theDegreeV) const
  {
    theDegreeU = 2;
    theDegreeV = 2;

    // 4 poles in U x 4 poles in V
    thePoles.Resize(1, 4, 1, 4, false);
    thePoles(1, 1) = gp_Pnt(0, 0, 0);
    thePoles(2, 1) = gp_Pnt(1, 0, 1);
    thePoles(3, 1) = gp_Pnt(2, 0, 0.5);
    thePoles(4, 1) = gp_Pnt(3, 0, 0);
    thePoles(1, 2) = gp_Pnt(0, 0.7, 1);
    thePoles(2, 2) = gp_Pnt(1, 0.7, 2);
    thePoles(3, 2) = gp_Pnt(2, 0.7, 1.5);
    thePoles(4, 2) = gp_Pnt(3, 0.7, 1);
    thePoles(1, 3) = gp_Pnt(0, 1.3, 0.5);
    thePoles(2, 3) = gp_Pnt(1, 1.3, 1.5);
    thePoles(3, 3) = gp_Pnt(2, 1.3, 1.0);
    thePoles(4, 3) = gp_Pnt(3, 1.3, 0.5);
    thePoles(1, 4) = gp_Pnt(0, 2, 0);
    thePoles(2, 4) = gp_Pnt(1, 2, 0.5);
    thePoles(3, 4) = gp_Pnt(2, 2, 0);
    thePoles(4, 4) = gp_Pnt(3, 2, 0);

    // U knots: 0, 1, 2, 3 with mults 3,1,1,3 -> flat: 0,0,0,1,2,3,3,3
    NCollection_Array1<double> aKnotsU(1, 4);
    aKnotsU(1) = 0.0;
    aKnotsU(2) = 1.0;
    aKnotsU(3) = 2.0;
    aKnotsU(4) = 3.0;
    NCollection_Array1<int> aMultsU(1, 4);
    aMultsU(1) = 3;
    aMultsU(2) = 1;
    aMultsU(3) = 1;
    aMultsU(4) = 3;
    theFlatKnotsU.Resize(1, 8, false);
    createFlatKnots(aKnotsU, aMultsU, theFlatKnotsU);

    // V knots: 0, 1, 2 with mults 3,1,3 -> flat: [0,0,0,1,2,2,2] -> 7 flat knots
    NCollection_Array1<double> aKnotsV(1, 3);
    aKnotsV(1) = 0.0;
    aKnotsV(2) = 1.0;
    aKnotsV(3) = 2.0;
    NCollection_Array1<int> aMultsV(1, 3);
    aMultsV(1) = 3;
    aMultsV(2) = 1;
    aMultsV(3) = 3;
    theFlatKnotsV.Resize(1, 7, false);
    createFlatKnots(aKnotsV, aMultsV, theFlatKnotsV);
  }
};

//==================================================================================================
// D0 correctness
//==================================================================================================

TEST_F(BSplSLib_CacheGridTest, D0_MatchesSingleCache)
{
  NCollection_Array2<gp_Pnt> aPoles;
  NCollection_Array1<double> aFlatKnotsU, aFlatKnotsV;
  int                        aDegU = 0, aDegV = 0;
  setupMultiSpanSurface(aPoles, aFlatKnotsU, aFlatKnotsV, aDegU, aDegV);

  occ::handle<BSplSLib_CacheGrid> aGrid =
    new BSplSLib_CacheGrid(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, aPoles, nullptr);
  occ::handle<BSplSLib_Cache> aSingleCache =
    new BSplSLib_Cache(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.5)
  {
    for (double v = 0.0; v <= 2.0; v += 0.5)
    {
      if (!aSingleCache->IsCacheValid(u, v))
        aSingleCache->BuildCache(u, v, aFlatKnotsU, aFlatKnotsV, aPoles, nullptr);

      gp_Pnt aGridPnt, aSinglePnt;
      aGrid->D0(u, v, aGridPnt);
      aSingleCache->D0(u, v, aSinglePnt);

      EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE) << "D0 at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE) << "D0 at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE) << "D0 at u=" << u << " v=" << v;
    }
  }
}

//==================================================================================================
// D1 correctness
//==================================================================================================

TEST_F(BSplSLib_CacheGridTest, D1_MatchesSingleCache)
{
  NCollection_Array2<gp_Pnt> aPoles;
  NCollection_Array1<double> aFlatKnotsU, aFlatKnotsV;
  int                        aDegU = 0, aDegV = 0;
  setupMultiSpanSurface(aPoles, aFlatKnotsU, aFlatKnotsV, aDegU, aDegV);

  occ::handle<BSplSLib_CacheGrid> aGrid =
    new BSplSLib_CacheGrid(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, aPoles, nullptr);
  occ::handle<BSplSLib_Cache> aSingleCache =
    new BSplSLib_Cache(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, nullptr);

  for (double u = 0.0; u <= 3.0; u += 0.7)
  {
    for (double v = 0.0; v <= 2.0; v += 0.7)
    {
      if (!aSingleCache->IsCacheValid(u, v))
        aSingleCache->BuildCache(u, v, aFlatKnotsU, aFlatKnotsV, aPoles, nullptr);

      gp_Pnt aGridPnt, aSinglePnt;
      gp_Vec aGridDU, aGridDV, aSingleDU, aSingleDV;
      aGrid->D1(u, v, aGridPnt, aGridDU, aGridDV);
      aSingleCache->D1(u, v, aSinglePnt, aSingleDU, aSingleDV);

      EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE)
        << "D1 point at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridDU.X(), aSingleDU.X(), THE_TOLERANCE)
        << "D1 tangentU at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridDV.Y(), aSingleDV.Y(), THE_TOLERANCE)
        << "D1 tangentV at u=" << u << " v=" << v;
    }
  }
}

//==================================================================================================
// D2 correctness
//==================================================================================================

TEST_F(BSplSLib_CacheGridTest, D2_MatchesSingleCache)
{
  NCollection_Array2<gp_Pnt> aPoles;
  NCollection_Array1<double> aFlatKnotsU, aFlatKnotsV;
  int                        aDegU = 0, aDegV = 0;
  setupMultiSpanSurface(aPoles, aFlatKnotsU, aFlatKnotsV, aDegU, aDegV);

  occ::handle<BSplSLib_CacheGrid> aGrid =
    new BSplSLib_CacheGrid(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, aPoles, nullptr);
  occ::handle<BSplSLib_Cache> aSingleCache =
    new BSplSLib_Cache(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, nullptr);

  for (double u = 0.1; u <= 2.9; u += 0.9)
  {
    for (double v = 0.1; v <= 1.9; v += 0.9)
    {
      if (!aSingleCache->IsCacheValid(u, v))
        aSingleCache->BuildCache(u, v, aFlatKnotsU, aFlatKnotsV, aPoles, nullptr);

      gp_Pnt aGridPnt, aSinglePnt;
      gp_Vec aGridDU, aGridDV, aGridD2U, aGridD2V, aGridD2UV;
      gp_Vec aSingleDU, aSingleDV, aSingleD2U, aSingleD2V, aSingleD2UV;
      aGrid->D2(u, v, aGridPnt, aGridDU, aGridDV, aGridD2U, aGridD2V, aGridD2UV);
      aSingleCache->D2(u, v, aSinglePnt, aSingleDU, aSingleDV, aSingleD2U, aSingleD2V, aSingleD2UV);

      EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE)
        << "D2 point at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridD2U.Z(), aSingleD2U.Z(), THE_TOLERANCE)
        << "D2 curvatureU at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridD2UV.Z(), aSingleD2UV.Z(), THE_TOLERANCE)
        << "D2 mixed at u=" << u << " v=" << v;
    }
  }
}

//==================================================================================================
// Span boundary oscillation
//==================================================================================================

TEST_F(BSplSLib_CacheGridTest, SpanBoundary_Oscillation)
{
  NCollection_Array2<gp_Pnt> aPoles;
  NCollection_Array1<double> aFlatKnotsU, aFlatKnotsV;
  int                        aDegU = 0, aDegV = 0;
  setupMultiSpanSurface(aPoles, aFlatKnotsU, aFlatKnotsV, aDegU, aDegV);

  occ::handle<BSplSLib_CacheGrid> aGrid =
    new BSplSLib_CacheGrid(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, aPoles, nullptr);
  occ::handle<BSplSLib_Cache> aSingleCache =
    new BSplSLib_Cache(aDegU, false, aFlatKnotsU, aDegV, false, aFlatKnotsV, nullptr);

  // First evaluate at center to set up the grid
  gp_Pnt aDummyPnt;
  aGrid->D0(1.5, 0.5, aDummyPnt);

  // Oscillate near U=1, V=1 boundary
  const double aUParams[] = {0.95, 1.05, 0.98, 1.02};
  const double aVParams[] = {0.95, 1.05, 0.98, 1.02};

  for (double u : aUParams)
  {
    for (double v : aVParams)
    {
      if (!aSingleCache->IsCacheValid(u, v))
        aSingleCache->BuildCache(u, v, aFlatKnotsU, aFlatKnotsV, aPoles, nullptr);

      gp_Pnt aGridPnt, aSinglePnt;
      aGrid->D0(u, v, aGridPnt);
      aSingleCache->D0(u, v, aSinglePnt);

      EXPECT_NEAR(aGridPnt.X(), aSinglePnt.X(), THE_TOLERANCE)
        << "Oscillation at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridPnt.Y(), aSinglePnt.Y(), THE_TOLERANCE)
        << "Oscillation at u=" << u << " v=" << v;
      EXPECT_NEAR(aGridPnt.Z(), aSinglePnt.Z(), THE_TOLERANCE)
        << "Oscillation at u=" << u << " v=" << v;
    }
  }
}
