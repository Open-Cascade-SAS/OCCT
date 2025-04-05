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

#include <NCollection_SparseArray.hxx>

#include <gtest/gtest.h>

// Basic test type for the SparseArray
typedef Standard_Integer ItemType;

// Custom class for testing complex types in the SparseArray
class TestClass
{
public:
  TestClass(int id = 0, double value = 0.0)
      : myId(id),
        myValue(value)
  {
  }

  bool operator==(const TestClass& other) const
  {
    return myId == other.myId && fabs(myValue - other.myValue) < 1e-10;
  }

  bool operator!=(const TestClass& other) const { return !(*this == other); }

  int GetId() const { return myId; }

  double GetValue() const { return myValue; }

  void SetValue(double value) { myValue = value; }

private:
  int    myId;
  double myValue;
};

TEST(NCollection_SparseArrayTest, BasicFunctions)
{
  // Test constructor with increment
  NCollection_SparseArray<ItemType> anArray(10);

  // Test initial state
  EXPECT_TRUE(anArray.IsEmpty());
  EXPECT_EQ(anArray.Size(), 0);
  EXPECT_EQ(anArray.Extent(), 0);

  // Test setting values
  anArray.SetValue(5, 55);
  anArray.SetValue(15, 155);

  EXPECT_FALSE(anArray.IsEmpty());
  EXPECT_EQ(anArray.Size(), 2);

  // Test value access
  EXPECT_EQ(anArray.Value(5), 55);
  EXPECT_EQ(anArray(15), 155);

  // Test HasValue (IsBound)
  EXPECT_TRUE(anArray.HasValue(5));
  EXPECT_TRUE(anArray.IsBound(15));
  EXPECT_FALSE(anArray.HasValue(10));
  EXPECT_FALSE(anArray.IsBound(20));

  // Test UnsetValue (UnBind)
  EXPECT_TRUE(anArray.UnsetValue(5));
  EXPECT_FALSE(anArray.HasValue(5));
  EXPECT_EQ(anArray.Size(), 1);

  // Test modifying a value
  anArray.ChangeValue(15) = 255;
  EXPECT_EQ(anArray.Value(15), 255);
}

TEST(NCollection_SparseArrayTest, LargeIndices)
{
  NCollection_SparseArray<ItemType> anArray(10);

  // Test with large indices
  const Standard_Size largeIndex1 = 1000;
  const Standard_Size largeIndex2 = 10000;

  anArray.SetValue(largeIndex1, 1000);
  anArray.SetValue(largeIndex2, 10000);

  EXPECT_EQ(anArray.Size(), 2);
  EXPECT_TRUE(anArray.HasValue(largeIndex1));
  EXPECT_TRUE(anArray.HasValue(largeIndex2));
  EXPECT_EQ(anArray.Value(largeIndex1), 1000);
  EXPECT_EQ(anArray.Value(largeIndex2), 10000);
}

TEST(NCollection_SparseArrayTest, SparseDistribution)
{
  NCollection_SparseArray<ItemType> anArray(8);

  // Set values with sparse distribution
  anArray.SetValue(3, 3);
  anArray.SetValue(10, 10);
  anArray.SetValue(17, 17);
  anArray.SetValue(100, 100);
  anArray.SetValue(1000, 1000);

  EXPECT_EQ(anArray.Size(), 5);
  EXPECT_TRUE(anArray.HasValue(3));
  EXPECT_TRUE(anArray.HasValue(10));
  EXPECT_TRUE(anArray.HasValue(17));
  EXPECT_TRUE(anArray.HasValue(100));
  EXPECT_TRUE(anArray.HasValue(1000));

  // Test gaps have no value
  EXPECT_FALSE(anArray.HasValue(4));
  EXPECT_FALSE(anArray.HasValue(11));
  EXPECT_FALSE(anArray.HasValue(101));
}

TEST(NCollection_SparseArrayTest, IteratorFunctions)
{
  NCollection_SparseArray<ItemType> anArray(8);

  // Set some values
  anArray.SetValue(5, 50);
  anArray.SetValue(10, 100);
  anArray.SetValue(20, 200);
  anArray.SetValue(30, 300);

  // Test const iterator
  NCollection_SparseArray<ItemType>::ConstIterator anIt(anArray);

  // Check that iterator finds all values in some order
  Standard_Boolean found5  = Standard_False;
  Standard_Boolean found10 = Standard_False;
  Standard_Boolean found20 = Standard_False;
  Standard_Boolean found30 = Standard_False;
  Standard_Size    count   = 0;

  for (; anIt.More(); anIt.Next(), ++count)
  {
    Standard_Size index = anIt.Index();
    ItemType      value = anIt.Value();

    if (index == 5 && value == 50)
      found5 = Standard_True;
    else if (index == 10 && value == 100)
      found10 = Standard_True;
    else if (index == 20 && value == 200)
      found20 = Standard_True;
    else if (index == 30 && value == 300)
      found30 = Standard_True;
  }

  EXPECT_EQ(count, 4);
  EXPECT_TRUE(found5);
  EXPECT_TRUE(found10);
  EXPECT_TRUE(found20);
  EXPECT_TRUE(found30);

  // Test non-const iterator
  NCollection_SparseArray<ItemType>::Iterator aModIt(anArray);

  // Modify values through iterator
  for (; aModIt.More(); aModIt.Next())
  {
    aModIt.ChangeValue() *= 2;
  }

  // Check modified values
  EXPECT_EQ(anArray.Value(5), 100);
  EXPECT_EQ(anArray.Value(10), 200);
  EXPECT_EQ(anArray.Value(20), 400);
  EXPECT_EQ(anArray.Value(30), 600);
}

TEST(NCollection_SparseArrayTest, ComplexType)
{
  NCollection_SparseArray<TestClass> anArray(10);

  // Set values with complex type
  anArray.SetValue(5, TestClass(5, 3.14));
  anArray.SetValue(10, TestClass(10, 2.718));

  EXPECT_EQ(anArray.Size(), 2);

  // Check values
  EXPECT_EQ(anArray.Value(5).GetId(), 5);
  EXPECT_DOUBLE_EQ(anArray.Value(5).GetValue(), 3.14);

  EXPECT_EQ(anArray.Value(10).GetId(), 10);
  EXPECT_DOUBLE_EQ(anArray.Value(10).GetValue(), 2.718);

  // Modify through ChangeValue
  anArray.ChangeValue(5).SetValue(6.28);
  EXPECT_DOUBLE_EQ(anArray.Value(5).GetValue(), 6.28);

  // Test unset with complex type
  EXPECT_TRUE(anArray.UnsetValue(5));
  EXPECT_FALSE(anArray.HasValue(5));
}

TEST(NCollection_SparseArrayTest, Clear)
{
  NCollection_SparseArray<ItemType> anArray(8);

  // Set some values
  anArray.SetValue(5, 5);
  anArray.SetValue(10, 10);
  anArray.SetValue(15, 15);

  EXPECT_EQ(anArray.Size(), 3);

  // Clear the array
  anArray.Clear();

  EXPECT_TRUE(anArray.IsEmpty());
  EXPECT_EQ(anArray.Size(), 0);
  EXPECT_FALSE(anArray.HasValue(5));
  EXPECT_FALSE(anArray.HasValue(10));
  EXPECT_FALSE(anArray.HasValue(15));
}

TEST(NCollection_SparseArrayTest, DataMapInterface)
{
  NCollection_SparseArray<ItemType> anArray(10);

  // Test Bind method
  anArray.Bind(5, 50);
  anArray.Bind(15, 150);

  EXPECT_TRUE(anArray.IsBound(5));
  EXPECT_TRUE(anArray.IsBound(15));

  // Test Find method
  EXPECT_EQ(anArray.Find(5), 50);
  EXPECT_EQ(anArray.Find(15), 150);

  // Test ChangeFind method
  anArray.ChangeFind(5) = 55;
  EXPECT_EQ(anArray.Find(5), 55);

  // Test UnBind method
  EXPECT_TRUE(anArray.UnBind(5));
  EXPECT_FALSE(anArray.IsBound(5));
  EXPECT_EQ(anArray.Size(), 1);
}

TEST(NCollection_SparseArrayTest, AssignAndExchange)
{
  // Create and populate first array
  NCollection_SparseArray<ItemType> anArray1(10);
  anArray1.SetValue(5, 50);
  anArray1.SetValue(15, 150);

  // Test Assign method
  NCollection_SparseArray<ItemType> anArray2(20);
  anArray2.SetValue(7, 70);

  anArray2.Assign(anArray1);

  EXPECT_EQ(anArray2.Size(), 2);
  EXPECT_TRUE(anArray2.HasValue(5));
  EXPECT_TRUE(anArray2.HasValue(15));
  EXPECT_EQ(anArray2.Value(5), 50);
  EXPECT_EQ(anArray2.Value(15), 150);
  EXPECT_FALSE(anArray2.HasValue(7)); // Should be removed after Assign

  // Test Exchange method
  NCollection_SparseArray<ItemType> anArray3(30);
  anArray3.SetValue(10, 100);
  anArray3.SetValue(20, 200);

  anArray2.Exchange(anArray3);

  // Check anArray2 after exchange (should have anArray3's values)
  EXPECT_EQ(anArray2.Size(), 2);
  EXPECT_TRUE(anArray2.HasValue(10));
  EXPECT_TRUE(anArray2.HasValue(20));
  EXPECT_EQ(anArray2.Value(10), 100);
  EXPECT_EQ(anArray2.Value(20), 200);
  EXPECT_FALSE(anArray2.HasValue(5));
  EXPECT_FALSE(anArray2.HasValue(15));

  // Check anArray3 after exchange (should have anArray2's values)
  EXPECT_EQ(anArray3.Size(), 2);
  EXPECT_TRUE(anArray3.HasValue(5));
  EXPECT_TRUE(anArray3.HasValue(15));
  EXPECT_EQ(anArray3.Value(5), 50);
  EXPECT_EQ(anArray3.Value(15), 150);
  EXPECT_FALSE(anArray3.HasValue(10));
  EXPECT_FALSE(anArray3.HasValue(20));
}
