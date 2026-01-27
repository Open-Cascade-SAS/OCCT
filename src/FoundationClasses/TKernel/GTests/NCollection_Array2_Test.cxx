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
  NCollection_Array2<int> anArray;
  EXPECT_EQ(0, anArray.Length());
  EXPECT_EQ(0, anArray.NbRows());
  EXPECT_EQ(0, anArray.NbColumns());
}

TEST(NCollection_Array2Test, ConstructorWithBounds)
{
  NCollection_Array2<int> anArray(1, 5, 1, 10);
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
  NCollection_Array2<int> anArray(-2, 2, -5, 5); // 5 rows, 11 cols
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
  NCollection_Array2<int> anArray(1, 3, 1, 4);
  for (int aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (int aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      anArray.SetValue(aRowIter, aColIter, aRowIter * 100 + aColIter);
    }
  }

  for (int aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (int aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      EXPECT_EQ(aRowIter * 100 + aColIter, anArray.Value(aRowIter, aColIter));
      EXPECT_EQ(aRowIter * 100 + aColIter, anArray(aRowIter, aColIter)); // Using operator()
    }
  }
}

TEST(NCollection_Array2Test, ChangeValueAccess)
{
  NCollection_Array2<int> anArray(0, 2, 0, 3);
  for (int aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (int aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
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
  NCollection_Array2<int> anArray(1, 5, 1, 5);
  anArray.Init(42);
  for (int aRowIter = anArray.LowerRow(); aRowIter <= anArray.UpperRow(); ++aRowIter)
  {
    for (int aColIter = anArray.LowerCol(); aColIter <= anArray.UpperCol(); ++aColIter)
    {
      EXPECT_EQ(42, anArray(aRowIter, aColIter));
    }
  }
}

// --- Copy and Move Semantics ---

TEST(NCollection_Array2Test, CopyConstructor)
{
  NCollection_Array2<int> anArray1(1, 3, 1, 4);
  anArray1.Init(123);

  NCollection_Array2<int> anArray2(anArray1);

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
  NCollection_Array2<int> anArray1(1, 3, 1, 4);
  anArray1.Init(123);

  NCollection_Array2<int> anArray2(1, 3, 1, 4);
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
  NCollection_Array2<int> anArray1(1, 5, 1, 10);
  anArray1.SetValue(3, 7, 123);

  // Move construct
  NCollection_Array2<int> anArray2(std::move(anArray1));

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
  NCollection_Array2<int> anArray1(1, 5, 1, 10);
  anArray1.SetValue(3, 7, 123);

  NCollection_Array2<int> anArray2;
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
  NCollection_Array2<int> anArray(1, 4, 1, 5); // 4x5 array
  for (int aRowIter = 1; aRowIter <= 4; ++aRowIter)
  {
    for (int aColIter = 1; aColIter <= 5; ++aColIter)
    {
      anArray(aRowIter, aColIter) = aRowIter * 100 + aColIter;
    }
  }

  // Resize to be larger, keeping data
  anArray.Resize(0, 5, 0, 6, true); // New size 6x7

  // Verify new dimensions
  EXPECT_EQ(6, anArray.NbRows());
  EXPECT_EQ(7, anArray.NbColumns());
  EXPECT_EQ(0, anArray.LowerRow());
  EXPECT_EQ(6, anArray.UpperCol());

  // Verify original data is preserved in the correct locations
  for (int aRowIter = 0; aRowIter <= 3; ++aRowIter)
  {
    for (int aColIter = 0; aColIter <= 4; ++aColIter)
    {
      EXPECT_EQ((aRowIter + 1) * 100 + (aColIter + 1), anArray(aRowIter, aColIter));
    }
  }
}

TEST(NCollection_Array2Test, ReIndex_UpdateBounds)
{
  NCollection_Array2<int> anArray(1, 5, 1, 10); // 5x10 array

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
  NCollection_Array2<int> anArray(1, 2, 1, 3); // 2x3 array
  for (int aRowIter = 1; aRowIter <= 2; ++aRowIter)
  {
    for (int aColIter = 1; aColIter <= 3; ++aColIter)
    {
      anArray(aRowIter, aColIter) = aRowIter * 10 + aColIter; // 11, 12, 13, 21, 22, 23
    }
  }

  // Test range-based for loop
  std::vector<int> aExpectedValues = {11, 12, 13, 21, 22, 23};
  int              anIndex         = 0;
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
  NCollection_Array2<int> anArray(1, 4, 1, 6); // 4x6 = 24 elements
  int                     anExpectedValue = 0;
  for (int aRowIter = 1; aRowIter <= 4; ++aRowIter)
  {
    for (int aColIter = 1; aColIter <= 6; ++aColIter)
    {
      anArray(aRowIter, aColIter) = anExpectedValue++;
    }
  }

  // Resize to 6x4 (24 elements), copying data
  anArray.Resize(1, 6, 1, 4, true);

  // Verify new dimensions
  EXPECT_EQ(6, anArray.NbRows());
  EXPECT_EQ(4, anArray.NbColumns());
  EXPECT_EQ(24, anArray.Length());

  // Verify the common 4x4 sub-matrix was not scrambled.
  // This will fail if the copy logic in Resize is incorrect.
  for (int anElemInd = anArray.Lower(); anElemInd < anArray.Lower() + 16; ++anElemInd)
  {
    EXPECT_EQ(anElemInd - anArray.Lower(),
              static_cast<NCollection_Array1<int>&>(anArray).Value(anElemInd));
  }
}

TEST(NCollection_Array2Test, ReIndex_Interference)
{
  // This test explicitly verifies that UpdateUpperRow modifies the LowerRow,
  // showing how the separate update methods can interfere with each other.
  NCollection_Array2<int> anArray(1, 10, 1, 1); // A 10x1 array
  const int               anInitialNbRows = anArray.NbRows();

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

// Helper struct for testing in-place construction with multiple arguments
struct Array2MultiArgType
{
  int    myA;
  double myB;
  Array2MultiArgType()
      : myA(0),
        myB(0.0)
  {
  }
  Array2MultiArgType(int theA, double theB)
      : myA(theA),
        myB(theB)
  {
  }
};

TEST(NCollection_Array2Test, EmplaceValue)
{
  NCollection_Array2<Array2MultiArgType> anArray(1, 3, 1, 4);

  // Test EmplaceValue with multiple constructor arguments
  Array2MultiArgType& aRef1 = anArray.EmplaceValue(1, 1, 42, 3.14);
  EXPECT_EQ(42, aRef1.myA);
  EXPECT_NEAR(3.14, aRef1.myB, 1e-10);

  Array2MultiArgType& aRef2 = anArray.EmplaceValue(2, 3, 100, 2.71);
  EXPECT_EQ(100, aRef2.myA);
  EXPECT_NEAR(2.71, aRef2.myB, 1e-10);

  // Verify the values are in the array
  EXPECT_EQ(42, anArray(1, 1).myA);
  EXPECT_EQ(100, anArray(2, 3).myA);

  // Verify other elements are default-constructed
  EXPECT_EQ(0, anArray(1, 2).myA);
  EXPECT_EQ(0, anArray(3, 4).myA);
}

TEST(NCollection_Array2Test, EmplaceValue_ReplacesExisting)
{
  NCollection_Array2<Array2MultiArgType> anArray(1, 2, 1, 2);

  // Set initial values
  anArray.EmplaceValue(1, 1, 11, 1.1);
  anArray.EmplaceValue(1, 2, 12, 1.2);
  anArray.EmplaceValue(2, 1, 21, 2.1);
  anArray.EmplaceValue(2, 2, 22, 2.2);

  // Replace value at (1, 2)
  Array2MultiArgType& aRef = anArray.EmplaceValue(1, 2, 120, 12.0);
  EXPECT_EQ(120, aRef.myA);
  EXPECT_NEAR(12.0, aRef.myB, 1e-10);

  // Verify other values unchanged
  EXPECT_EQ(11, anArray(1, 1).myA);
  EXPECT_EQ(120, anArray(1, 2).myA);
  EXPECT_EQ(21, anArray(2, 1).myA);
  EXPECT_EQ(22, anArray(2, 2).myA);
}