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
#include <NCollection_Array2.hxx>

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

TEST(BSplSLibTest, UnitWeights_SingleElement)
{
  const NCollection_Array2<double> aWeights = BSplSLib::UnitWeights(1, 1);

  EXPECT_EQ(aWeights.Size(), 1);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1, 1), 1.0);
}
