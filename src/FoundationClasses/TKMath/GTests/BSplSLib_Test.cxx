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

#include <BSplSLib.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_Array2.hxx>

namespace
{
void initKnots(NCollection_Array1<double>& theKnots)
{
  for (int anIndex = theKnots.Lower(); anIndex <= theKnots.Upper(); ++anIndex)
  {
    theKnots.SetValue(anIndex, static_cast<double>(anIndex - theKnots.Lower()));
  }
}
} // namespace

//=================================================================================================

TEST(BSplSLibTest, UnitWeights_SmallSurface_ReturnsNonOwning)
{
  const int                        aNbU     = 4;
  const int                        aNbV     = 5;
  const NCollection_Array2<double> aWeights = BSplSLib::UnitWeights(aNbU, aNbV);

  EXPECT_EQ(aWeights.ColLength(), aNbU);
  EXPECT_EQ(aWeights.RowLength(), aNbV);
  EXPECT_EQ(aWeights.Size(), aNbU * aNbV);
  EXPECT_FALSE(aWeights.IsDeletable());

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      EXPECT_DOUBLE_EQ(aWeights(i, j), 1.0);
    }
  }
}

//=================================================================================================

TEST(BSplSLibTest, UnitWeights_MaxBezierSize_ReturnsNonOwning)
{
  // Max Bezier: 26x26 = 676 poles, well within limit
  const int                        aNbU     = 26;
  const int                        aNbV     = 26;
  const NCollection_Array2<double> aWeights = BSplSLib::UnitWeights(aNbU, aNbV);

  EXPECT_EQ(aWeights.Size(), aNbU * aNbV);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1, 1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(aNbU, aNbV), 1.0);
}

//=================================================================================================

TEST(BSplSLibTest, UnitWeights_AtMaxLimit_ReturnsNonOwning)
{
  // Exactly at the limit: 2049 total poles
  const int                        aNbU     = 3;
  const int                        aNbV     = 683;
  const NCollection_Array2<double> aWeights = BSplSLib::UnitWeights(aNbU, aNbV);

  EXPECT_EQ(aWeights.Size(), aNbU * aNbV);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1, 1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(aNbU, aNbV), 1.0);
}

//=================================================================================================

TEST(BSplSLibTest, UnitWeights_OverMaxLimit_ReturnsOwning)
{
  // Over the limit: needs allocation
  const int                        aNbU     = 50;
  const int                        aNbV     = 50;
  const NCollection_Array2<double> aWeights = BSplSLib::UnitWeights(aNbU, aNbV);

  EXPECT_EQ(aWeights.ColLength(), aNbU);
  EXPECT_EQ(aWeights.RowLength(), aNbV);
  EXPECT_EQ(aWeights.Size(), aNbU * aNbV);
  EXPECT_TRUE(aWeights.IsDeletable());

  for (int i = 1; i <= aNbU; ++i)
  {
    for (int j = 1; j <= aNbV; ++j)
    {
      EXPECT_DOUBLE_EQ(aWeights(i, j), 1.0);
    }
  }
}

//=================================================================================================

TEST(BSplSLibTest, UnitWeights_SingleElement)
{
  const NCollection_Array2<double> aWeights = BSplSLib::UnitWeights(1, 1);

  EXPECT_EQ(aWeights.Size(), 1);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1, 1), 1.0);
}

//=================================================================================================

TEST(BSplSLibTest, InteriorIntervalIsPreserved)
{
  NCollection_Array1<double> aKnots(1, 5);
  initKnots(aKnots);
  const BSplSLib::LocalSpan aSpan = BSplSLib::SelectLocalSpan(1.5, 0, 2, 3, 1, 5, aKnots, false);
  EXPECT_DOUBLE_EQ(aSpan.Parameter, 1.5);
  EXPECT_EQ(aSpan.First, 2);
  EXPECT_EQ(aSpan.Last, 3);
  EXPECT_FALSE(aSpan.IsKnot);
}

//=================================================================================================

TEST(BSplSLibTest, InteriorKnotSelectsRequestedSide)
{
  NCollection_Array1<double> aKnots(1, 5);
  initKnots(aKnots);

  const BSplSLib::LocalSpan aLeft = BSplSLib::SelectLocalSpan(2.0, -1, 3, 3, 1, 5, aKnots, false);
  EXPECT_EQ(aLeft.First, 2);
  EXPECT_EQ(aLeft.Last, 3);
  EXPECT_TRUE(aLeft.IsKnot);

  const BSplSLib::LocalSpan aDefault = BSplSLib::SelectLocalSpan(2.0, 0, 3, 3, 1, 5, aKnots, false);
  EXPECT_EQ(aDefault.First, 3);
  EXPECT_EQ(aDefault.Last, 4);
  EXPECT_TRUE(aDefault.IsKnot);

  const BSplSLib::LocalSpan aRight = BSplSLib::SelectLocalSpan(2.0, 1, 3, 3, 1, 5, aKnots, false);
  EXPECT_EQ(aRight.First, 3);
  EXPECT_EQ(aRight.Last, 4);
  EXPECT_TRUE(aRight.IsKnot);
}

//=================================================================================================

TEST(BSplSLibTest, BoundariesSelectOnlyValidAdjacentSpan)
{
  NCollection_Array1<double> aKnots(1, 5);
  initKnots(aKnots);

  for (const int aSide : {-1, 0, 1})
  {
    const BSplSLib::LocalSpan aFirst =
      BSplSLib::SelectLocalSpan(aKnots.Value(1), aSide, 1, 1, 1, 5, aKnots, false);
    EXPECT_EQ(aFirst.First, 1);
    EXPECT_EQ(aFirst.Last, 2);

    const BSplSLib::LocalSpan aLast =
      BSplSLib::SelectLocalSpan(aKnots.Value(5), aSide, 5, 5, 1, 5, aKnots, false);
    EXPECT_EQ(aLast.First, 4);
    EXPECT_EQ(aLast.Last, 5);
  }
}

//=================================================================================================

TEST(BSplSLibTest, PeriodicSeamMapsToRequestedBoundary)
{
  NCollection_Array1<double> aKnots(1, 5);
  initKnots(aKnots);

  const BSplSLib::LocalSpan aLeft =
    BSplSLib::SelectLocalSpan(aKnots.Value(1), -1, 1, 1, 1, 5, aKnots, true);
  EXPECT_DOUBLE_EQ(aLeft.Parameter, aKnots.Value(5));
  EXPECT_EQ(aLeft.First, 4);
  EXPECT_EQ(aLeft.Last, 5);
  EXPECT_TRUE(aLeft.IsKnot);

  const BSplSLib::LocalSpan aRight =
    BSplSLib::SelectLocalSpan(aKnots.Value(5), 1, 5, 5, 1, 5, aKnots, true);
  EXPECT_DOUBLE_EQ(aRight.Parameter, aKnots.Value(1));
  EXPECT_EQ(aRight.First, 1);
  EXPECT_EQ(aRight.Last, 2);
  EXPECT_TRUE(aRight.IsKnot);
}

//=================================================================================================

TEST(BSplSLibTest, LocatedIntervalIsClampedToActiveKnotRange)
{
  NCollection_Array1<double> aKnots(1, 5);
  initKnots(aKnots);

  const BSplSLib::LocalSpan aBefore = BSplSLib::SelectLocalSpan(-0.5, 0, 0, 1, 1, 5, aKnots, false);
  EXPECT_EQ(aBefore.First, 1);
  EXPECT_EQ(aBefore.Last, 2);

  const BSplSLib::LocalSpan anAfter = BSplSLib::SelectLocalSpan(4.5, 0, 5, 6, 1, 5, aKnots, false);
  EXPECT_EQ(anAfter.First, 4);
  EXPECT_EQ(anAfter.Last, 5);
}

//=================================================================================================

TEST(BSplSLibTest, ReversedLocatedRangeUsesLastAdjacentSpan)
{
  NCollection_Array1<double> aKnots(1, 7);
  initKnots(aKnots);

  const BSplSLib::LocalSpan aSpan = BSplSLib::SelectLocalSpan(2.5, 0, 4, 3, 1, 7, aKnots, false);
  EXPECT_DOUBLE_EQ(aSpan.Parameter, 2.5);
  EXPECT_EQ(aSpan.First, 2);
  EXPECT_EQ(aSpan.Last, 3);
  EXPECT_FALSE(aSpan.IsKnot);
}