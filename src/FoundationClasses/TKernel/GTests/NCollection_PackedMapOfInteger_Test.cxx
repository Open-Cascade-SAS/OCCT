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

#include <NCollection_PackedMapOfInteger.hxx>

#include <gtest/gtest.h>

#include <vector>

//==================================================================================================
// Basic Construction Tests
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, DefaultConstructor)
{
  NCollection_PackedMapOfInteger aMap;
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
}

TEST(NCollection_PackedMapOfIntegerTest, ConstructorWithBuckets)
{
  NCollection_PackedMapOfInteger aMap(100);
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
}

TEST(NCollection_PackedMapOfIntegerTest, CopyConstructor)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(10);
  aMap1.Add(100);

  NCollection_PackedMapOfInteger aMap2(aMap1);

  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.Contains(1));
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(100));

  // Ensure deep copy - modifying original shouldn't affect copy
  aMap1.Add(1000);
  EXPECT_EQ(aMap1.Extent(), 4);
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_FALSE(aMap2.Contains(1000));
}

TEST(NCollection_PackedMapOfIntegerTest, MoveConstructor)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(10);
  aMap1.Add(100);

  NCollection_PackedMapOfInteger aMap2(std::move(aMap1));

  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.Contains(1));
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(100));

  // Original should be empty after move
  EXPECT_TRUE(aMap1.IsEmpty());
}

TEST(NCollection_PackedMapOfIntegerTest, CopyAssignment)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(5);
  aMap1.Add(15);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(100);
  aMap2.Add(200);
  aMap2.Add(300);

  aMap2 = aMap1;

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(5));
  EXPECT_TRUE(aMap2.Contains(15));
  EXPECT_FALSE(aMap2.Contains(100));

  // Ensure deep copy
  aMap1.Add(25);
  EXPECT_FALSE(aMap2.Contains(25));
}

TEST(NCollection_PackedMapOfIntegerTest, MoveAssignment)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(5);
  aMap1.Add(15);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(100);

  aMap2 = std::move(aMap1);

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(5));
  EXPECT_TRUE(aMap2.Contains(15));
  EXPECT_TRUE(aMap1.IsEmpty());
}

//==================================================================================================
// Basic Operations Tests
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, AddAndContains)
{
  NCollection_PackedMapOfInteger aMap;

  // Adding new elements should return true
  EXPECT_TRUE(aMap.Add(10));
  EXPECT_TRUE(aMap.Add(20));
  EXPECT_TRUE(aMap.Add(30));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_FALSE(aMap.Contains(40));
}

TEST(NCollection_PackedMapOfIntegerTest, AddDuplicate)
{
  NCollection_PackedMapOfInteger aMap;

  EXPECT_TRUE(aMap.Add(10));
  EXPECT_EQ(aMap.Extent(), 1);

  // Adding duplicate should return false
  EXPECT_FALSE(aMap.Add(10));
  EXPECT_EQ(aMap.Extent(), 1);
}

TEST(NCollection_PackedMapOfIntegerTest, Remove)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_EQ(aMap.Extent(), 3);

  // Removing existing element should return true
  EXPECT_TRUE(aMap.Remove(20));
  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_FALSE(aMap.Contains(20));

  // Removing non-existing element should return false
  EXPECT_FALSE(aMap.Remove(20));
  EXPECT_FALSE(aMap.Remove(40));
  EXPECT_EQ(aMap.Extent(), 2);
}

TEST(NCollection_PackedMapOfIntegerTest, Clear)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  EXPECT_EQ(aMap.Extent(), 3);

  aMap.Clear();

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);

  // Should be able to add elements again
  aMap.Add(100);
  EXPECT_EQ(aMap.Extent(), 1);
  EXPECT_TRUE(aMap.Contains(100));
}

//==================================================================================================
// Edge Cases and Special Values
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, ZeroValue)
{
  NCollection_PackedMapOfInteger aMap;

  EXPECT_TRUE(aMap.Add(0));
  EXPECT_TRUE(aMap.Contains(0));
  EXPECT_EQ(aMap.Extent(), 1);

  EXPECT_TRUE(aMap.Remove(0));
  EXPECT_FALSE(aMap.Contains(0));
  EXPECT_TRUE(aMap.IsEmpty());
}

TEST(NCollection_PackedMapOfIntegerTest, NegativeValues)
{
  NCollection_PackedMapOfInteger aMap;

  aMap.Add(-1);
  aMap.Add(-100);
  aMap.Add(-1000);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(-1));
  EXPECT_TRUE(aMap.Contains(-100));
  EXPECT_TRUE(aMap.Contains(-1000));
  EXPECT_FALSE(aMap.Contains(1));

  EXPECT_TRUE(aMap.Remove(-100));
  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_FALSE(aMap.Contains(-100));
}

TEST(NCollection_PackedMapOfIntegerTest, MixedPositiveNegative)
{
  NCollection_PackedMapOfInteger aMap;

  aMap.Add(-50);
  aMap.Add(0);
  aMap.Add(50);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(-50));
  EXPECT_TRUE(aMap.Contains(0));
  EXPECT_TRUE(aMap.Contains(50));
}

TEST(NCollection_PackedMapOfIntegerTest, ConsecutiveValues)
{
  // Test packed storage of consecutive values within same block
  NCollection_PackedMapOfInteger aMap;

  for (int i = 0; i < 32; ++i)
  {
    aMap.Add(i);
  }

  EXPECT_EQ(aMap.Extent(), 32);

  for (int i = 0; i < 32; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
  }
}

TEST(NCollection_PackedMapOfIntegerTest, SparseValues)
{
  NCollection_PackedMapOfInteger aMap;

  // Values spread across different blocks
  aMap.Add(0);
  aMap.Add(100);
  aMap.Add(1000);
  aMap.Add(10000);
  aMap.Add(100000);

  EXPECT_EQ(aMap.Extent(), 5);

  EXPECT_TRUE(aMap.Contains(0));
  EXPECT_TRUE(aMap.Contains(100));
  EXPECT_TRUE(aMap.Contains(1000));
  EXPECT_TRUE(aMap.Contains(10000));
  EXPECT_TRUE(aMap.Contains(100000));
}

//==================================================================================================
// Min/Max Queries
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, GetMinimalMapped)
{
  NCollection_PackedMapOfInteger aMap;

  aMap.Add(50);
  aMap.Add(10);
  aMap.Add(100);
  aMap.Add(5);
  aMap.Add(75);

  EXPECT_EQ(aMap.GetMinimalMapped(), 5);

  aMap.Remove(5);
  EXPECT_EQ(aMap.GetMinimalMapped(), 10);
}

TEST(NCollection_PackedMapOfIntegerTest, GetMaximalMapped)
{
  NCollection_PackedMapOfInteger aMap;

  aMap.Add(50);
  aMap.Add(10);
  aMap.Add(100);
  aMap.Add(5);
  aMap.Add(75);

  EXPECT_EQ(aMap.GetMaximalMapped(), 100);

  aMap.Remove(100);
  EXPECT_EQ(aMap.GetMaximalMapped(), 75);
}

TEST(NCollection_PackedMapOfIntegerTest, MinMaxWithNegatives)
{
  NCollection_PackedMapOfInteger aMap;

  aMap.Add(-100);
  aMap.Add(-50);
  aMap.Add(0);
  aMap.Add(50);
  aMap.Add(100);

  EXPECT_EQ(aMap.GetMinimalMapped(), -100);
  EXPECT_EQ(aMap.GetMaximalMapped(), 100);
}

//==================================================================================================
// Boolean Operations - Union
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, Union)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(3);
  aMap2.Add(4);
  aMap2.Add(5);

  NCollection_PackedMapOfInteger aResult;
  aResult.Union(aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 5);
  EXPECT_TRUE(aResult.Contains(1));
  EXPECT_TRUE(aResult.Contains(2));
  EXPECT_TRUE(aResult.Contains(3));
  EXPECT_TRUE(aResult.Contains(4));
  EXPECT_TRUE(aResult.Contains(5));
}

TEST(NCollection_PackedMapOfIntegerTest, Unite)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(3);
  aMap2.Add(4);
  aMap2.Add(5);

  // Unite should return true when map is changed
  EXPECT_TRUE(aMap1.Unite(aMap2));

  EXPECT_EQ(aMap1.Extent(), 5);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(2));
  EXPECT_TRUE(aMap1.Contains(3));
  EXPECT_TRUE(aMap1.Contains(4));
  EXPECT_TRUE(aMap1.Contains(5));

  // Unite with same elements should return false
  EXPECT_FALSE(aMap1.Unite(aMap2));
}

TEST(NCollection_PackedMapOfIntegerTest, UniteOperator)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);

  aMap1 |= aMap2;

  EXPECT_EQ(aMap1.Extent(), 3);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(2));
  EXPECT_TRUE(aMap1.Contains(3));
}

//==================================================================================================
// Boolean Operations - Intersection
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, Intersection)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);
  aMap1.Add(4);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(5);
  aMap2.Add(6);

  NCollection_PackedMapOfInteger aResult;
  aResult.Intersection(aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 2);
  EXPECT_TRUE(aResult.Contains(2));
  EXPECT_TRUE(aResult.Contains(3));
  EXPECT_FALSE(aResult.Contains(1));
  EXPECT_FALSE(aResult.Contains(4));
  EXPECT_FALSE(aResult.Contains(5));
}

TEST(NCollection_PackedMapOfIntegerTest, Intersect)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);
  aMap1.Add(4);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(5);

  // Intersect should return true when map is changed
  EXPECT_TRUE(aMap1.Intersect(aMap2));

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(2));
  EXPECT_TRUE(aMap1.Contains(3));
}

TEST(NCollection_PackedMapOfIntegerTest, IntersectOperator)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(4);

  aMap1 &= aMap2;

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(2));
  EXPECT_TRUE(aMap1.Contains(3));
}

TEST(NCollection_PackedMapOfIntegerTest, IntersectionDisjoint)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(3);
  aMap2.Add(4);

  NCollection_PackedMapOfInteger aResult;
  aResult.Intersection(aMap1, aMap2);

  EXPECT_TRUE(aResult.IsEmpty());
}

//==================================================================================================
// Boolean Operations - Subtraction
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, Subtraction)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);
  aMap1.Add(4);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(5);

  NCollection_PackedMapOfInteger aResult;
  aResult.Subtraction(aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 2);
  EXPECT_TRUE(aResult.Contains(1));
  EXPECT_TRUE(aResult.Contains(4));
  EXPECT_FALSE(aResult.Contains(2));
  EXPECT_FALSE(aResult.Contains(3));
}

TEST(NCollection_PackedMapOfIntegerTest, Subtract)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);
  aMap1.Add(4);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(5);

  EXPECT_TRUE(aMap1.Subtract(aMap2));

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(4));
}

TEST(NCollection_PackedMapOfIntegerTest, SubtractOperator)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);

  aMap1 -= aMap2;

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(3));
  EXPECT_FALSE(aMap1.Contains(2));
}

//==================================================================================================
// Boolean Operations - Difference (XOR)
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, Difference)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(4);

  NCollection_PackedMapOfInteger aResult;
  aResult.Difference(aMap1, aMap2);

  // XOR: elements in one but not both
  EXPECT_EQ(aResult.Extent(), 2);
  EXPECT_TRUE(aResult.Contains(1));
  EXPECT_TRUE(aResult.Contains(4));
  EXPECT_FALSE(aResult.Contains(2));
  EXPECT_FALSE(aResult.Contains(3));
}

TEST(NCollection_PackedMapOfIntegerTest, Differ)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(4);

  EXPECT_TRUE(aMap1.Differ(aMap2));

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(4));
}

TEST(NCollection_PackedMapOfIntegerTest, DifferOperator)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(2);
  aMap2.Add(3);

  aMap1 ^= aMap2;

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(3));
  EXPECT_FALSE(aMap1.Contains(2));
}

//==================================================================================================
// Comparison Operations
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, IsEqual)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(1);
  aMap2.Add(2);
  aMap2.Add(3);

  EXPECT_TRUE(aMap1.IsEqual(aMap2));
  EXPECT_TRUE(aMap1 == aMap2);

  aMap2.Add(4);
  EXPECT_FALSE(aMap1.IsEqual(aMap2));
  EXPECT_FALSE(aMap1 == aMap2);
}

TEST(NCollection_PackedMapOfIntegerTest, IsEqualEmpty)
{
  NCollection_PackedMapOfInteger aMap1;
  NCollection_PackedMapOfInteger aMap2;

  EXPECT_TRUE(aMap1.IsEqual(aMap2));
  EXPECT_TRUE(aMap1 == aMap2);
}

TEST(NCollection_PackedMapOfIntegerTest, IsSubset)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(1);
  aMap2.Add(2);
  aMap2.Add(3);
  aMap2.Add(4);

  EXPECT_TRUE(aMap1.IsSubset(aMap2));
  EXPECT_TRUE(aMap1 <= aMap2);
  EXPECT_FALSE(aMap2.IsSubset(aMap1));
}

TEST(NCollection_PackedMapOfIntegerTest, IsSubsetSame)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(1);
  aMap2.Add(2);

  // Equal sets are subsets of each other
  EXPECT_TRUE(aMap1.IsSubset(aMap2));
  EXPECT_TRUE(aMap2.IsSubset(aMap1));
}

TEST(NCollection_PackedMapOfIntegerTest, IsSubsetEmpty)
{
  NCollection_PackedMapOfInteger aEmptyMap;
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);

  // Empty set is subset of any set
  EXPECT_TRUE(aEmptyMap.IsSubset(aMap));
  EXPECT_TRUE(aEmptyMap.IsSubset(aEmptyMap));
}

TEST(NCollection_PackedMapOfIntegerTest, HasIntersection)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(3);
  aMap2.Add(4);
  aMap2.Add(5);

  EXPECT_TRUE(aMap1.HasIntersection(aMap2));

  NCollection_PackedMapOfInteger aMap3;
  aMap3.Add(10);
  aMap3.Add(20);

  EXPECT_FALSE(aMap1.HasIntersection(aMap3));
}

//==================================================================================================
// Iterator Tests
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, IteratorBasic)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  std::vector<int> aValues;
  for (NCollection_PackedMapOfInteger::Iterator anIt(aMap); anIt.More(); anIt.Next())
  {
    aValues.push_back(anIt.Key());
  }

  EXPECT_EQ(aValues.size(), 3);

  // Check all values are present (order not guaranteed)
  EXPECT_NE(std::find(aValues.begin(), aValues.end(), 10), aValues.end());
  EXPECT_NE(std::find(aValues.begin(), aValues.end(), 20), aValues.end());
  EXPECT_NE(std::find(aValues.begin(), aValues.end(), 30), aValues.end());
}

TEST(NCollection_PackedMapOfIntegerTest, IteratorEmpty)
{
  NCollection_PackedMapOfInteger aMap;

  int aCount = 0;
  for (NCollection_PackedMapOfInteger::Iterator anIt(aMap); anIt.More(); anIt.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, 0);
}

TEST(NCollection_PackedMapOfIntegerTest, IteratorReinitialize)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(100);
  aMap2.Add(200);
  aMap2.Add(300);

  NCollection_PackedMapOfInteger::Iterator anIt(aMap1);

  int aCount1 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount1;
  }
  EXPECT_EQ(aCount1, 2);

  // Reinitialize with different map
  anIt.Initialize(aMap2);

  int aCount2 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount2;
  }
  EXPECT_EQ(aCount2, 3);
}

TEST(NCollection_PackedMapOfIntegerTest, IteratorReset)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);

  NCollection_PackedMapOfInteger::Iterator anIt(aMap);

  // First pass
  int aCount1 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount1;
  }
  EXPECT_EQ(aCount1, 2);

  // Reset and iterate again
  anIt.Reset();

  int aCount2 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount2;
  }
  EXPECT_EQ(aCount2, 2);
}

//==================================================================================================
// Resize and Performance Tests
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, ReSize)
{
  NCollection_PackedMapOfInteger aMap(1); // Start with minimal bucket count

  // Add many elements to trigger resize
  for (int i = 0; i < 1000; ++i)
  {
    aMap.Add(i);
  }

  EXPECT_EQ(aMap.Extent(), 1000);

  // Verify all elements are present
  for (int i = 0; i < 1000; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
  }
}

TEST(NCollection_PackedMapOfIntegerTest, LargeValues)
{
  NCollection_PackedMapOfInteger aMap;

  aMap.Add(1000000);
  aMap.Add(2000000);
  aMap.Add(3000000);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(1000000));
  EXPECT_TRUE(aMap.Contains(2000000));
  EXPECT_TRUE(aMap.Contains(3000000));
}

//==================================================================================================
// Self-Operations Tests
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, UnionWithSelf)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  NCollection_PackedMapOfInteger aResult;
  aResult.Union(aMap, aMap);

  EXPECT_EQ(aResult.Extent(), 3);
  EXPECT_TRUE(aResult == aMap);
}

TEST(NCollection_PackedMapOfIntegerTest, IntersectionWithSelf)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  NCollection_PackedMapOfInteger aResult;
  aResult.Intersection(aMap, aMap);

  EXPECT_EQ(aResult.Extent(), 3);
  EXPECT_TRUE(aResult == aMap);
}

TEST(NCollection_PackedMapOfIntegerTest, SubtractionFromSelf)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  NCollection_PackedMapOfInteger aResult;
  aResult.Subtraction(aMap, aMap);

  EXPECT_TRUE(aResult.IsEmpty());
}

TEST(NCollection_PackedMapOfIntegerTest, DifferenceWithSelf)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  NCollection_PackedMapOfInteger aResult;
  aResult.Difference(aMap, aMap);

  EXPECT_TRUE(aResult.IsEmpty());
}

//==================================================================================================
// Result as Operand Tests
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, UnionResultAsOperand)
{
  NCollection_PackedMapOfInteger aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_PackedMapOfInteger aMap2;
  aMap2.Add(3);
  aMap2.Add(4);

  // Result can be same as operand
  aMap1.Union(aMap1, aMap2);

  EXPECT_EQ(aMap1.Extent(), 4);
  EXPECT_TRUE(aMap1.Contains(1));
  EXPECT_TRUE(aMap1.Contains(2));
  EXPECT_TRUE(aMap1.Contains(3));
  EXPECT_TRUE(aMap1.Contains(4));
}

//==================================================================================================
// Empty Map Operations
//==================================================================================================

TEST(NCollection_PackedMapOfIntegerTest, UnionWithEmpty)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);

  NCollection_PackedMapOfInteger aEmpty;

  NCollection_PackedMapOfInteger aResult;
  aResult.Union(aMap, aEmpty);

  EXPECT_TRUE(aResult == aMap);
}

TEST(NCollection_PackedMapOfIntegerTest, IntersectionWithEmpty)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);

  NCollection_PackedMapOfInteger aEmpty;

  NCollection_PackedMapOfInteger aResult;
  aResult.Intersection(aMap, aEmpty);

  EXPECT_TRUE(aResult.IsEmpty());
}

TEST(NCollection_PackedMapOfIntegerTest, SubtractionEmpty)
{
  NCollection_PackedMapOfInteger aMap;
  aMap.Add(1);
  aMap.Add(2);

  NCollection_PackedMapOfInteger aEmpty;

  NCollection_PackedMapOfInteger aResult;
  aResult.Subtraction(aMap, aEmpty);

  EXPECT_TRUE(aResult == aMap);

  // Subtract full from empty
  NCollection_PackedMapOfInteger aResult2;
  aResult2.Subtraction(aEmpty, aMap);

  EXPECT_TRUE(aResult2.IsEmpty());
}
