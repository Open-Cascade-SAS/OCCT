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

#include <NCollection_Array1.hxx>
#include <Standard_Integer.hxx>

#include <gtest/gtest.h>

// Test fixture for NCollection_Array1 tests
class NCollection_Array1Test : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NCollection_Array1Test, DefaultConstructor)
{
  // Default constructor should not compile as it's explicitly deleted
  // NCollection_Array1<Standard_Integer> anArray;
  // Instead we need to use the parameterized constructor
  NCollection_Array1<Standard_Integer> anArray(1, 10);

  EXPECT_EQ(10, anArray.Length());
  EXPECT_EQ(1, anArray.Lower());
  EXPECT_EQ(10, anArray.Upper());
}

TEST_F(NCollection_Array1Test, ConstructorWithBounds)
{
  // Test constructor with explicit bounds
  NCollection_Array1<Standard_Integer> anArray(1, 5);
  EXPECT_EQ(5, anArray.Size());
  EXPECT_EQ(1, anArray.Lower());
  EXPECT_EQ(5, anArray.Upper());
}

TEST_F(NCollection_Array1Test, ConstructorWithNegativeBounds)
{
  // Test constructor with negative bounds
  NCollection_Array1<Standard_Integer> anArray(-3, 2);
  EXPECT_EQ(6, anArray.Size());
  EXPECT_EQ(-3, anArray.Lower());
  EXPECT_EQ(2, anArray.Upper());
}

TEST_F(NCollection_Array1Test, AssignmentValue)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);

  // Initialize array
  for (Standard_Integer i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    anArray(i) = i * 10;
  }

  // Verify values
  for (Standard_Integer i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    EXPECT_EQ(i * 10, anArray(i));
  }

  // Test Value vs operator()
  for (Standard_Integer i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    EXPECT_EQ(anArray.Value(i), anArray(i));
  }
}

TEST_F(NCollection_Array1Test, CopyConstructor)
{
  NCollection_Array1<Standard_Integer> anArray1(1, 5);

  // Initialize array
  for (Standard_Integer i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    anArray1(i) = i * 10;
  }

  // Copy construct
  NCollection_Array1<Standard_Integer> anArray2(anArray1);

  // Verify copy
  EXPECT_EQ(anArray1.Length(), anArray2.Length());
  EXPECT_EQ(anArray1.Lower(), anArray2.Lower());
  EXPECT_EQ(anArray1.Upper(), anArray2.Upper());

  for (Standard_Integer i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    EXPECT_EQ(anArray1(i), anArray2(i));
  }

  // Modify original to ensure deep copy
  anArray1(3) = 999;
  EXPECT_NE(anArray1(3), anArray2(3));
}

TEST_F(NCollection_Array1Test, ValueAccess)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);

  // Set values
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    anArray.SetValue(i, i * 10);
  }

  // Check values
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray.Value(i));
    EXPECT_EQ(i * 10, anArray(i)); // Using operator()
  }
}

TEST_F(NCollection_Array1Test, ChangeValueAccess)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);

  // Set values using ChangeValue
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    anArray.ChangeValue(i) = i * 10;
  }

  // Check values
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray(i));
  }

  // Modify values through references
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    anArray.ChangeValue(i) += 5;
  }

  // Check modified values
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10 + 5, anArray(i));
  }
}

TEST_F(NCollection_Array1Test, AssignmentOperator)
{
  NCollection_Array1<Standard_Integer> anArray1(1, 5);

  // Initialize array
  for (Standard_Integer i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    anArray1(i) = i * 10;
  }

  // Test assignment
  NCollection_Array1<Standard_Integer> anArray2(11, 15);
  anArray2 = anArray1;
  anArray2.Resize(1, 5, Standard_True); // Resize to match anArray1

  // Verify assignment result
  EXPECT_EQ(anArray1.Length(), anArray2.Length());
  EXPECT_EQ(anArray1.Lower(), anArray2.Lower());
  EXPECT_EQ(anArray1.Upper(), anArray2.Upper());

  for (Standard_Integer i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    EXPECT_EQ(anArray1(i), anArray2(i));
  }
}

TEST_F(NCollection_Array1Test, Move)
{
  NCollection_Array1<Standard_Integer> anArray1(1, 5);

  // Initialize array
  for (Standard_Integer i = anArray1.Lower(); i <= anArray1.Upper(); i++)
  {
    anArray1(i) = i * 10;
  }

  // Test Move method
  NCollection_Array1<Standard_Integer> anArray2(11, 15);
  anArray2.Move(anArray1);
  anArray2.Resize(1, 5, Standard_True); // Resize to match anArray1

  // Verify move result
  EXPECT_EQ(5, anArray2.Length());
  EXPECT_EQ(1, anArray2.Lower());
  EXPECT_EQ(5, anArray2.Upper());

  // Original array is keep referecing the same data
  EXPECT_EQ(anArray1.Lower(), anArray2.Lower());
  EXPECT_EQ(anArray1.Upper(), anArray2.Upper());
  EXPECT_EQ(anArray1(1), anArray2(1));
  EXPECT_EQ(anArray1(2), anArray2(2));
  EXPECT_EQ(5, anArray1.Length());
}

TEST_F(NCollection_Array1Test, Init)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);

  // Test Init method
  anArray.Init(42);

  // Verify all values initialized
  for (Standard_Integer i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    EXPECT_EQ(42, anArray(i));
  }
}

TEST_F(NCollection_Array1Test, SetValue)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);

  // Test SetValue method
  anArray.SetValue(3, 123);

  // Verify value set
  EXPECT_EQ(123, anArray(3));
}

TEST_F(NCollection_Array1Test, FirstLast)
{
  NCollection_Array1<Standard_Integer> anArray(5, 10);

  anArray(5)  = 55;
  anArray(10) = 1010;

  // Test First and Last methods
  EXPECT_EQ(55, anArray.First());
  EXPECT_EQ(1010, anArray.Last());

  // Change First and Last
  anArray.ChangeFirst() = 555;
  anArray.ChangeLast()  = 10101;

  EXPECT_EQ(555, anArray.First());
  EXPECT_EQ(10101, anArray.Last());
}

TEST_F(NCollection_Array1Test, STLIteration)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);
  for (Standard_Integer i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    anArray(i) = i * 10;
  }

  // Test STL-style iteration
  Standard_Integer index = 1;
  for (const auto& val : anArray)
  {
    EXPECT_EQ(index * 10, val);
    index++;
  }
}

TEST_F(NCollection_Array1Test, Resize)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);
  for (Standard_Integer i = anArray.Lower(); i <= anArray.Upper(); i++)
  {
    anArray(i) = i * 10;
  }

  // Test Resize method - increasing size
  anArray.Resize(1, 10, Standard_True);

  // Check new size
  EXPECT_EQ(10, anArray.Length());
  EXPECT_EQ(1, anArray.Lower());
  EXPECT_EQ(10, anArray.Upper());

  // Verify original data preserved
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray(i));
  }

  // Test Resize method - decreasing size
  NCollection_Array1<Standard_Integer> anArray2(1, 10);
  for (Standard_Integer i = anArray2.Lower(); i <= anArray2.Upper(); i++)
  {
    anArray2(i) = i * 10;
  }

  anArray2.Resize(1, 5, Standard_True);

  // Check new size
  EXPECT_EQ(5, anArray2.Length());
  EXPECT_EQ(1, anArray2.Lower());
  EXPECT_EQ(5, anArray2.Upper());

  // Verify original data preserved
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    EXPECT_EQ(i * 10, anArray2(i));
  }
}

TEST_F(NCollection_Array1Test, ChangeValue)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);
  anArray.Init(42);

  // Test ChangeValue method
  anArray.ChangeValue(3) = 123;

  // Verify value changed
  EXPECT_EQ(123, anArray(3));

  // Verify other values unchanged
  EXPECT_EQ(42, anArray(1));
  EXPECT_EQ(42, anArray(2));
  EXPECT_EQ(42, anArray(4));
  EXPECT_EQ(42, anArray(5));
}

TEST_F(NCollection_Array1Test, IteratorAccess)
{
  NCollection_Array1<Standard_Integer> anArray(1, 5);
  for (Standard_Integer i = 1; i <= 5; i++)
  {
    anArray(i) = i * 10;
  }

  // Test iteration using STL-compatible iterators
  int index = 1;
  for (auto it = anArray.begin(); it != anArray.end(); ++it, ++index)
  {
    EXPECT_EQ(index * 10, *it);
  }

  // Test range-based for loop
  index = 1;
  for (const auto& value : anArray)
  {
    EXPECT_EQ(index * 10, value);
    index++;
  }
}
