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

#include <BSplCLib.hxx>
#include <NCollection_Array1.hxx>

TEST(BSplCLibTest, UnitWeights_SmallSize_ReturnsNonOwning)
{
  const int                        aNbElems = 10;
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(aNbElems);

  EXPECT_EQ(aWeights.Lower(), 1);
  EXPECT_EQ(aWeights.Upper(), aNbElems);
  EXPECT_EQ(aWeights.Length(), aNbElems);
  EXPECT_FALSE(aWeights.IsDeletable());

  for (int i = 1; i <= aNbElems; ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
}

TEST(BSplCLibTest, UnitWeights_MaxSize_ReturnsNonOwning)
{
  const int                        aNbElems = BSplCLib::MaxUnitWeightsSize();
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(aNbElems);

  EXPECT_EQ(aWeights.Length(), aNbElems);
  EXPECT_FALSE(aWeights.IsDeletable());

  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
  EXPECT_DOUBLE_EQ(aWeights(aNbElems), 1.0);
}

TEST(BSplCLibTest, UnitWeights_OverMaxSize_ReturnsOwning)
{
  const int                        aNbElems = BSplCLib::MaxUnitWeightsSize() + 1;
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(aNbElems);

  EXPECT_EQ(aWeights.Lower(), 1);
  EXPECT_EQ(aWeights.Upper(), aNbElems);
  EXPECT_EQ(aWeights.Length(), aNbElems);
  EXPECT_TRUE(aWeights.IsDeletable());

  for (int i = 1; i <= aNbElems; ++i)
  {
    EXPECT_DOUBLE_EQ(aWeights(i), 1.0);
  }
}

TEST(BSplCLibTest, UnitWeights_SingleElement)
{
  const NCollection_Array1<double> aWeights = BSplCLib::UnitWeights(1);

  EXPECT_EQ(aWeights.Length(), 1);
  EXPECT_FALSE(aWeights.IsDeletable());
  EXPECT_DOUBLE_EQ(aWeights(1), 1.0);
}

TEST(BSplCLibTest, MaxUnitWeightsSize_IsPositive)
{
  EXPECT_GT(BSplCLib::MaxUnitWeightsSize(), 0);
  EXPECT_EQ(BSplCLib::MaxUnitWeightsSize(), 2049);
}
