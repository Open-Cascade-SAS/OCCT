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

#include <math_DoubleTab.hxx>

#include <gtest/gtest.h>

#include <Standard_Real.hxx>
#include <Standard_Integer.hxx>
#include <Precision.hxx>

// Tests for constructors
TEST(MathDoubleTabTest, DefaultConstructor)
{
  // Test standard constructor with ranges
  math_DoubleTab aTab(1, 3, 1, 3);

  // Initialize with test values
  aTab.Init(5.0);

  // Check all values are set to 5.0
  for (Standard_Integer anI = 1; anI <= 3; anI++)
  {
    for (Standard_Integer aJ = 1; aJ <= 3; aJ++)
    {
      EXPECT_DOUBLE_EQ(aTab.Value(anI, aJ), 5.0);
      EXPECT_DOUBLE_EQ(aTab(anI, aJ), 5.0); // Test operator()
    }
  }
}

TEST(MathDoubleTabTest, ExternalArrayConstructor)
{
  // Create external array
  Standard_Real anArray[9] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0};

  // Create math_DoubleTab from external array
  math_DoubleTab aTab(anArray, 1, 3, 1, 3);

  // Check values are correctly accessed
  Standard_Integer anIndex = 0;
  for (Standard_Integer anI = 1; anI <= 3; anI++)
  {
    for (Standard_Integer aJ = 1; aJ <= 3; aJ++)
    {
      EXPECT_DOUBLE_EQ(aTab.Value(anI, aJ), anArray[anIndex]);
      anIndex++;
    }
  }
}

TEST(MathDoubleTabTest, CopyConstructor)
{
  // Create original array
  math_DoubleTab aOriginal(1, 3, 1, 3);
  aOriginal.Init(10.0);

  // Set some specific values
  aOriginal.Value(1, 1) = 1.5;
  aOriginal.Value(2, 2) = 2.5;
  aOriginal.Value(3, 3) = 3.5;

  // Create copy
  math_DoubleTab aCopy(aOriginal);

  // Check values are copied correctly
  EXPECT_DOUBLE_EQ(aCopy.Value(1, 1), 1.5);
  EXPECT_DOUBLE_EQ(aCopy.Value(2, 2), 2.5);
  EXPECT_DOUBLE_EQ(aCopy.Value(3, 3), 3.5);
  EXPECT_DOUBLE_EQ(aCopy.Value(1, 2), 10.0); // Check other values too
}

// Tests for basic operations
TEST(MathDoubleTabTest, InitOperation)
{
  math_DoubleTab aTab(0, 2, 0, 2);

  // Initialize with specific value
  aTab.Init(7.5);

  // Check all elements are set
  for (Standard_Integer anI = 0; anI <= 2; anI++)
  {
    for (Standard_Integer aJ = 0; aJ <= 2; aJ++)
    {
      EXPECT_DOUBLE_EQ(aTab.Value(anI, aJ), 7.5);
    }
  }
}

TEST(MathDoubleTabTest, ValueAccess)
{
  math_DoubleTab aTab(1, 2, 1, 2);

  // Set values using Value method
  aTab.Value(1, 1) = 11.0;
  aTab.Value(1, 2) = 12.0;
  aTab.Value(2, 1) = 21.0;
  aTab.Value(2, 2) = 22.0;

  // Check values using operator()
  EXPECT_DOUBLE_EQ(aTab(1, 1), 11.0);
  EXPECT_DOUBLE_EQ(aTab(1, 2), 12.0);
  EXPECT_DOUBLE_EQ(aTab(2, 1), 21.0);
  EXPECT_DOUBLE_EQ(aTab(2, 2), 22.0);
}

TEST(MathDoubleTabTest, OperatorAccess)
{
  math_DoubleTab aTab(-1, 1, -1, 1);

  // Set values using operator()
  aTab(-1, -1) = -11.0;
  aTab(-1, 0)  = -10.0;
  aTab(-1, 1)  = -9.0;
  aTab(0, -1)  = -1.0;
  aTab(0, 0)   = 0.0;
  aTab(0, 1)   = 1.0;
  aTab(1, -1)  = 9.0;
  aTab(1, 0)   = 10.0;
  aTab(1, 1)   = 11.0;

  // Check values using Value method
  EXPECT_DOUBLE_EQ(aTab.Value(-1, -1), -11.0);
  EXPECT_DOUBLE_EQ(aTab.Value(-1, 0), -10.0);
  EXPECT_DOUBLE_EQ(aTab.Value(-1, 1), -9.0);
  EXPECT_DOUBLE_EQ(aTab.Value(0, -1), -1.0);
  EXPECT_DOUBLE_EQ(aTab.Value(0, 0), 0.0);
  EXPECT_DOUBLE_EQ(aTab.Value(0, 1), 1.0);
  EXPECT_DOUBLE_EQ(aTab.Value(1, -1), 9.0);
  EXPECT_DOUBLE_EQ(aTab.Value(1, 0), 10.0);
  EXPECT_DOUBLE_EQ(aTab.Value(1, 1), 11.0);
}

TEST(MathDoubleTabTest, CopyMethod)
{
  // Create source array
  math_DoubleTab aSource(1, 2, 1, 2);
  aSource.Value(1, 1) = 100.0;
  aSource.Value(1, 2) = 200.0;
  aSource.Value(2, 1) = 300.0;
  aSource.Value(2, 2) = 400.0;

  // Create destination array
  math_DoubleTab aDest(1, 2, 1, 2);
  aDest.Init(0.0);

  // Copy from source to destination
  aSource.Copy(aDest);

  // Check values are copied
  EXPECT_DOUBLE_EQ(aDest.Value(1, 1), 100.0);
  EXPECT_DOUBLE_EQ(aDest.Value(1, 2), 200.0);
  EXPECT_DOUBLE_EQ(aDest.Value(2, 1), 300.0);
  EXPECT_DOUBLE_EQ(aDest.Value(2, 2), 400.0);
}

// Tests for buffer optimization
TEST(MathDoubleTabTest, SmallArrayOptimization)
{
  // Test that small arrays (<=16 elements) use buffer optimization
  // 4x4 = 16 elements, should use buffer
  math_DoubleTab aSmallTab(1, 4, 1, 4);
  aSmallTab.Init(42.0);

  // Set and read values to ensure it works correctly
  aSmallTab.Value(2, 3) = 123.45;
  EXPECT_DOUBLE_EQ(aSmallTab.Value(2, 3), 123.45);
  EXPECT_DOUBLE_EQ(aSmallTab.Value(1, 1), 42.0);
}

TEST(MathDoubleTabTest, LargeArrayAllocation)
{
  // Test that large arrays (>16 elements) allocate memory
  // 5x5 = 25 elements, should allocate
  math_DoubleTab aLargeTab(1, 5, 1, 5);
  aLargeTab.Init(99.99);

  // Set and read values to ensure it works correctly
  aLargeTab.Value(3, 4) = 987.65;
  EXPECT_DOUBLE_EQ(aLargeTab.Value(3, 4), 987.65);
  EXPECT_DOUBLE_EQ(aLargeTab.Value(1, 1), 99.99);
}

// Tests for edge cases
TEST(MathDoubleTabTest, SingleElement)
{
  // Test with single element array
  math_DoubleTab aSingle(5, 5, 10, 10);
  aSingle.Value(5, 10) = 777.0;

  EXPECT_DOUBLE_EQ(aSingle.Value(5, 10), 777.0);
  EXPECT_DOUBLE_EQ(aSingle(5, 10), 777.0);
}

TEST(MathDoubleTabTest, NegativeIndices)
{
  // Test with negative indices
  math_DoubleTab aNegTab(-5, -1, -3, -1);
  aNegTab.Init(-1.0);

  aNegTab.Value(-3, -2) = -999.0;
  EXPECT_DOUBLE_EQ(aNegTab.Value(-3, -2), -999.0);
  EXPECT_DOUBLE_EQ(aNegTab.Value(-5, -3), -1.0);
}
