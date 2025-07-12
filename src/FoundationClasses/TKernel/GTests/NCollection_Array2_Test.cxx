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

#include <NCollection_Array2.hxx>
#include <Standard_Integer.hxx>

#include <gtest/gtest.h>

// --- Constructor Tests ---

TEST(NCollection_Array2Test, DefaultConstructor)
{
  NCollection_Array2<Standard_Integer> anArray;
  EXPECT_EQ(0, anArray.Length());
  EXPECT_EQ(0, anArray.NbRows());
  EXPECT_EQ(0, anArray.NbColumns());
}

TEST(NCollection_Array2Test, ConstructorWithBounds)
{
  NCollection_Array2<Standard_Integer> anArray(1, 5, 1, 10);
  EXPECT_EQ(50, anArray.Length());
  EXPECT_EQ(5, anArray.NbRows());
  EXPECT_EQ(10, anArray.NbColumns());
  EXPECT_EQ(1, anArray.LowerRow());
  EXPECT_EQ(5, anArray.UpperRow());
  EXPECT_EQ(1, anArray.LowerCol());
  EXPECT_EQ(10, anArray.UpperCol());
}

TEST(NCollection_Array2Test, ConstructorWithNegativeBounds)
{
  NCollection_Array2<Standard_Integer> anArray(-2, 2, -5, 5); // 5 rows, 11 cols
  EXPECT_EQ(55, anArray.Length());
  EXPECT_EQ(5, anArray.NbRows());
  EXPECT_EQ(11, anArray.NbColumns());
  EXPECT_EQ(-2, anArray.LowerRow());
  EXPECT_EQ(2, anArray.UpperRow());
  EXPECT_EQ(-5, anArray.LowerCol());
  EXPECT_EQ(5, anArray.UpperCol());
}

// --- Data Access and Manipulation ---

TEST(NCollection_Array2Test, ValueAccess)
{
  NCollection_Array2<Standard_Integer> anArray(1, 3, 1, 4);
  for (Standard_Integer aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (Standard_Integer aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      anArray.SetValue(aRowIter, aColIter, aRowIter * 100 + aColIter);
    }
  }

  for (Standard_Integer aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (Standard_Integer aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      EXPECT_EQ(aRowIter * 100 + aColIter, anArray.Value(aRowIter, aColIter));
      EXPECT_EQ(aRowIter * 100 + aColIter, anArray(aRowIter, aColIter)); // Using operator()
    }
  }
}

TEST(NCollection_Array2Test, ChangeValueAccess)
{
  NCollection_Array2<Standard_Integer> anArray(0, 2, 0, 3);
  for (Standard_Integer aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (Standard_Integer aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      anArray.ChangeValue(aRowIter, aColIter) = aRowIter * 100 + aColIter;
    }
  }

  // Verify initial values
  EXPECT_EQ(102, anArray(1, 2));

  // Modify a value
  anArray.ChangeValue(1, 2) = 999;
  EXPECT_EQ(999, anArray(1, 2));
}

TEST(NCollection_Array2Test, Init)
{
  NCollection_Array2<Standard_Integer> anArray(1, 5, 1, 5);
  anArray.Init(42);
  for (Standard_Integer aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (Standard_Integer aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      EXPECT_EQ(42, anArray(aRowIter, aColIter));
    }
  }
}

// --- Copy and Move Semantics ---

TEST(NCollection_Array2Test, CopyConstructor)
{
  NCollection_Array2<Standard_Integer> anArray1(1, 3, 1, 4);
  anArray1.Init(123);

  NCollection_Array2<Standard_Integer> anArray2(anArray1);

  // Verify dimensions and data are copied
  EXPECT_EQ(anArray1.Length(), anArray2.Length());
  EXPECT_EQ(anArray1.NbRows(), anArray2.NbRows());
  EXPECT_EQ(anArray1.NbColumns(), anArray2.NbColumns());
  EXPECT_EQ(anArray1(2, 3), anArray2(2, 3));

  // Modify original to ensure it was a deep copy
  anArray1.SetValue(2, 3, 999);
  EXPECT_EQ(123, anArray2(2, 3));
  EXPECT_NE(anArray1(2, 3), anArray2(2, 3));
}

TEST(NCollection_Array2Test, AssignmentOperator)
{
  NCollection_Array2<Standard_Integer> anArray1(1, 3, 1, 4);
  anArray1.Init(123);

  NCollection_Array2<Standard_Integer> anArray2(1, 3, 1, 4);
  anArray2.Init(0);

  anArray2 = anArray1; // Assign

  // Verify data is copied
  EXPECT_EQ(123, anArray2(2, 3));

  // Modify original to ensure it was a deep copy
  anArray1.SetValue(2, 3, 999);
  EXPECT_EQ(123, anArray2(2, 3));
}

TEST(NCollection_Array2Test, MoveConstructor)
{
  NCollection_Array2<Standard_Integer> anArray1(1, 5, 1, 10);
  anArray1.SetValue(3, 7, 123);

  // Move construct
  NCollection_Array2<Standard_Integer> anArray2(std::move(anArray1));

  // Verify new array has the data and dimensions
  EXPECT_EQ(50, anArray2.Length());
  EXPECT_EQ(5, anArray2.NbRows());
  EXPECT_EQ(10, anArray2.NbColumns());
  EXPECT_EQ(1, anArray2.LowerRow());
  EXPECT_EQ(10, anArray2.UpperCol());
  EXPECT_EQ(123, anArray2(3, 7));

  // Verify the moved-from array is empty
  EXPECT_EQ(0, anArray1.Length());
  EXPECT_EQ(0, anArray1.NbRows());
}

TEST(NCollection_Array2Test, MoveAssignment)
{
  NCollection_Array2<Standard_Integer> anArray1(1, 5, 1, 10);
  anArray1.SetValue(3, 7, 123);

  NCollection_Array2<Standard_Integer> anArray2;
  anArray2 = std::move(anArray1); // Move assignment

  // Verify new array has the data and dimensions
  EXPECT_EQ(50, anArray2.Length());
  EXPECT_EQ(5, anArray2.NbRows());
  EXPECT_EQ(123, anArray2(3, 7));

  // Verify the moved-from array is empty
  EXPECT_EQ(0, anArray1.Length());
}

// --- Resizing and Re-indexing ---

TEST(NCollection_Array2Test, Resize)
{
  NCollection_Array2<Standard_Integer> anArray(1, 4, 1, 5); // 4x5 array
  for (Standard_Integer aRowIter = 1; aRowIter <= 4; ++aRowIter)
  {
    for (Standard_Integer aColIter = 1; aColIter <= 5; ++aColIter)
    {
      anArray(aRowIter, aColIter) = aRowIter * 100 + aColIter;
    }
  }

  // Resize to be larger, keeping data
  anArray.Resize(0, 5, 0, 6, Standard_True); // New size 6x7

  // Verify new dimensions
  EXPECT_EQ(6, anArray.NbRows());
  EXPECT_EQ(7, anArray.NbColumns());
  EXPECT_EQ(0, anArray.LowerRow());
  EXPECT_EQ(6, anArray.UpperCol());

  // Verify original data is preserved in the correct locations
  for (Standard_Integer aRowIter = 0; aRowIter <= 3; ++aRowIter)
  {
    for (Standard_Integer aColIter = 0; aColIter <= 4; ++aColIter)
    {
      EXPECT_EQ((aRowIter + 1) * 100 + (aColIter + 1), anArray(aRowIter, aColIter));
    }
  }
}

TEST(NCollection_Array2Test, ReIndex_UpdateBounds)
{
  NCollection_Array2<Standard_Integer> anArray(1, 5, 1, 10); // 5x10 array

  // Test updating lower bounds
  anArray.UpdateLowerRow(0);
  anArray.UpdateLowerCol(0);
  EXPECT_EQ(0, anArray.LowerRow());
  EXPECT_EQ(4, anArray.UpperRow()); // 0 + 5 - 1
  EXPECT_EQ(0, anArray.LowerCol());
  EXPECT_EQ(9, anArray.UpperCol()); // 0 + 10 - 1
  EXPECT_EQ(5, anArray.NbRows());   // Size should not change
  EXPECT_EQ(10, anArray.NbColumns());

  // Test updating upper bounds (which shifts the lower bounds)
  anArray.UpdateUpperRow(10); // Request upper row to be 10
  anArray.UpdateUpperCol(20); // Request upper col to be 20
  EXPECT_EQ(10, anArray.UpperRow());
  EXPECT_EQ(20, anArray.UpperCol());
  EXPECT_EQ(6, anArray.LowerRow());  // Lower is now 10 - 5 + 1
  EXPECT_EQ(11, anArray.LowerCol()); // Lower is now 20 - 10 + 1
  EXPECT_EQ(5, anArray.NbRows());    // Size should not change
  EXPECT_EQ(10, anArray.NbColumns());
}

// --- Iteration ---

TEST(NCollection_Array2Test, STLIteration)
{
  NCollection_Array2<Standard_Integer> anArray(1, 2, 1, 3); // 2x3 array
  for (Standard_Integer aRowIter = 1; aRowIter <= 2; ++aRowIter)
  {
    for (Standard_Integer aColIter = 1; aColIter <= 3; ++aColIter)
    {
      anArray(aRowIter, aColIter) = aRowIter * 10 + aColIter; // 11, 12, 13, 21, 22, 23
    }
  }

  // Test range-based for loop
  std::vector<Standard_Integer> aExpectedValues = {11, 12, 13, 21, 22, 23};
  int                           anIndex         = 0;
  for (const auto& aValue : anArray)
  {
    EXPECT_EQ(aExpectedValues[anIndex++], aValue);
  }
  EXPECT_EQ(6, anIndex); // Ensure all elements were visited
}

TEST(NCollection_Array2Test, Resize_ChangeShapeSameSize)
{
  // This test checks for data scrambling when resizing to a different shape
  // with the same total number of elements.
  NCollection_Array2<Standard_Integer> anArray(1, 4, 1, 6); // 4x6 = 24 elements
  Standard_Integer                     anExpectedValue = 0;
  for (Standard_Integer aRowIter = 1; aRowIter <= 4; ++aRowIter)
  {
    for (Standard_Integer aColIter = 1; aColIter <= 6; ++aColIter)
    {
      anArray(aRowIter, aColIter) = anExpectedValue++;
    }
  }

  // Resize to 6x4 (24 elements), copying data
  anArray.Resize(1, 6, 1, 4, Standard_True);

  // Verify new dimensions
  EXPECT_EQ(6, anArray.NbRows());
  EXPECT_EQ(4, anArray.NbColumns());
  EXPECT_EQ(24, anArray.Length());

  // Verify the common 4x4 sub-matrix was not scrambled.
  // This will fail if the copy logic in Resize is incorrect.
  for (Standard_Integer anElemInd = anArray.Lower(); anElemInd < anArray.Lower() + 16; ++anElemInd)
  {
    EXPECT_EQ(anElemInd - anArray.Lower(),
              static_cast<NCollection_Array1<Standard_Integer>&>(anArray).Value(anElemInd));
  }
}

TEST(NCollection_Array2Test, ReIndex_Interference)
{
  // This test explicitly verifies that UpdateUpperRow modifies the LowerRow,
  // showing how the separate update methods can interfere with each other.
  NCollection_Array2<Standard_Integer> anArray(1, 10, 1, 1); // A 10x1 array
  const Standard_Integer               anInitialNbRows = anArray.NbRows();

  // 1. User sets a new lower bound.
  anArray.UpdateLowerRow(5);
  EXPECT_EQ(5, anArray.LowerRow());
  EXPECT_EQ(14, anArray.UpperRow()); // Upper bound is now 5 + 10 - 1

  // 2. User then sets a new upper bound, expecting the lower bound to stay at 5.
  anArray.UpdateUpperRow(12);

  // 3. Verify the final state.
  EXPECT_EQ(12, anArray.UpperRow()); // The upper bound is set as requested.

  // Verify that the lower bound was changed to accommodate the new upper bound,
  // undoing the previous call to UpdateLowerRow.
  EXPECT_EQ(3, anArray.LowerRow()); // Lower bound is now 12 - 10 + 1 = 3
  EXPECT_NE(5, anArray.LowerRow()); // It is no longer 5.

  // The total number of rows should never change during these operations.
  EXPECT_EQ(anInitialNbRows, anArray.NbRows());
}