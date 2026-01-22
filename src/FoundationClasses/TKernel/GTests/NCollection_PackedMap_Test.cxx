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

#include <NCollection_PackedMap.hxx>
#include <NCollection_PackedMapAlgo.hxx>

#include <gtest/gtest.h>

#include <cstdint>
#include <vector>

//==================================================================================================
// Type-parameterized tests for NCollection_PackedMap
//==================================================================================================

template <typename T>
class NCollection_PackedMapTypedTest : public testing::Test
{
protected:
  using MapType = NCollection_PackedMap<T>;
};

using TestedTypes = ::testing::Types<int, unsigned int, long, int64_t, size_t>;

TYPED_TEST_SUITE(NCollection_PackedMapTypedTest, TestedTypes);

//==================================================================================================
// Basic Construction Tests
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, DefaultConstructor)
{
  typename TestFixture::MapType aMap;
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
}

TYPED_TEST(NCollection_PackedMapTypedTest, ConstructorWithBuckets)
{
  typename TestFixture::MapType aMap(100);
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
}

TYPED_TEST(NCollection_PackedMapTypedTest, CopyConstructor)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(10));
  aMap1.Add(TypeParam(100));

  typename TestFixture::MapType aMap2(aMap1);

  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap2.Contains(TypeParam(10)));
  EXPECT_TRUE(aMap2.Contains(TypeParam(100)));

  // Ensure deep copy
  aMap1.Add(TypeParam(1000));
  EXPECT_EQ(aMap1.Extent(), 4);
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_FALSE(aMap2.Contains(TypeParam(1000)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, MoveConstructor)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(10));
  aMap1.Add(TypeParam(100));

  typename TestFixture::MapType aMap2(std::move(aMap1));

  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap2.Contains(TypeParam(10)));
  EXPECT_TRUE(aMap2.Contains(TypeParam(100)));
  EXPECT_TRUE(aMap1.IsEmpty());
}

TYPED_TEST(NCollection_PackedMapTypedTest, CopyAssignment)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(5));
  aMap1.Add(TypeParam(15));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(100));
  aMap2.Add(TypeParam(200));

  aMap2 = aMap1;

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(TypeParam(5)));
  EXPECT_TRUE(aMap2.Contains(TypeParam(15)));
  EXPECT_FALSE(aMap2.Contains(TypeParam(100)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, MoveAssignment)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(5));
  aMap1.Add(TypeParam(15));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(100));

  aMap2 = std::move(aMap1);

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(TypeParam(5)));
  EXPECT_TRUE(aMap2.Contains(TypeParam(15)));
  EXPECT_TRUE(aMap1.IsEmpty());
}

//==================================================================================================
// Basic Operations Tests
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, AddAndContains)
{
  typename TestFixture::MapType aMap;

  EXPECT_TRUE(aMap.Add(TypeParam(10)));
  EXPECT_TRUE(aMap.Add(TypeParam(20)));
  EXPECT_TRUE(aMap.Add(TypeParam(30)));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(TypeParam(10)));
  EXPECT_TRUE(aMap.Contains(TypeParam(20)));
  EXPECT_TRUE(aMap.Contains(TypeParam(30)));
  EXPECT_FALSE(aMap.Contains(TypeParam(40)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, AddDuplicate)
{
  typename TestFixture::MapType aMap;

  EXPECT_TRUE(aMap.Add(TypeParam(10)));
  EXPECT_EQ(aMap.Extent(), 1);

  EXPECT_FALSE(aMap.Add(TypeParam(10)));
  EXPECT_EQ(aMap.Extent(), 1);
}

TYPED_TEST(NCollection_PackedMapTypedTest, Remove)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(10));
  aMap.Add(TypeParam(20));
  aMap.Add(TypeParam(30));

  EXPECT_EQ(aMap.Extent(), 3);

  EXPECT_TRUE(aMap.Remove(TypeParam(20)));
  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_FALSE(aMap.Contains(TypeParam(20)));

  EXPECT_FALSE(aMap.Remove(TypeParam(20)));
  EXPECT_FALSE(aMap.Remove(TypeParam(40)));
  EXPECT_EQ(aMap.Extent(), 2);
}

TYPED_TEST(NCollection_PackedMapTypedTest, Clear)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));
  aMap.Add(TypeParam(3));

  EXPECT_EQ(aMap.Extent(), 3);

  aMap.Clear();

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);

  aMap.Add(TypeParam(100));
  EXPECT_EQ(aMap.Extent(), 1);
  EXPECT_TRUE(aMap.Contains(TypeParam(100)));
}

//==================================================================================================
// Edge Cases and Special Values
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, ZeroValue)
{
  typename TestFixture::MapType aMap;

  EXPECT_TRUE(aMap.Add(TypeParam(0)));
  EXPECT_TRUE(aMap.Contains(TypeParam(0)));
  EXPECT_EQ(aMap.Extent(), 1);

  EXPECT_TRUE(aMap.Remove(TypeParam(0)));
  EXPECT_FALSE(aMap.Contains(TypeParam(0)));
  EXPECT_TRUE(aMap.IsEmpty());
}

TYPED_TEST(NCollection_PackedMapTypedTest, ConsecutiveValues)
{
  typename TestFixture::MapType aMap;
  constexpr int                 aNbValues = 64;

  for (int i = 0; i < aNbValues; ++i)
  {
    aMap.Add(static_cast<TypeParam>(i));
  }

  EXPECT_EQ(aMap.Extent(), aNbValues);

  for (int i = 0; i < aNbValues; ++i)
  {
    EXPECT_TRUE(aMap.Contains(static_cast<TypeParam>(i)));
  }
}

TYPED_TEST(NCollection_PackedMapTypedTest, SparseValues)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(0));
  aMap.Add(TypeParam(100));
  aMap.Add(TypeParam(1000));
  aMap.Add(TypeParam(10000));
  aMap.Add(TypeParam(100000));

  EXPECT_EQ(aMap.Extent(), 5);

  EXPECT_TRUE(aMap.Contains(TypeParam(0)));
  EXPECT_TRUE(aMap.Contains(TypeParam(100)));
  EXPECT_TRUE(aMap.Contains(TypeParam(1000)));
  EXPECT_TRUE(aMap.Contains(TypeParam(10000)));
  EXPECT_TRUE(aMap.Contains(TypeParam(100000)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, BlockBoundary)
{
  // Test values around block boundaries (31, 32, 33 for 32-bit, 63, 64, 65 for 64-bit)
  typename TestFixture::MapType aMap;
  constexpr int                 aBitsPerBlock = NCollection_PackedMap<TypeParam>::BitsPerBlock;

  aMap.Add(static_cast<TypeParam>(aBitsPerBlock - 1));
  aMap.Add(static_cast<TypeParam>(aBitsPerBlock));
  aMap.Add(static_cast<TypeParam>(aBitsPerBlock + 1));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(static_cast<TypeParam>(aBitsPerBlock - 1)));
  EXPECT_TRUE(aMap.Contains(static_cast<TypeParam>(aBitsPerBlock)));
  EXPECT_TRUE(aMap.Contains(static_cast<TypeParam>(aBitsPerBlock + 1)));
}

//==================================================================================================
// Min/Max Queries
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, GetMinimalMapped)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(50));
  aMap.Add(TypeParam(10));
  aMap.Add(TypeParam(100));
  aMap.Add(TypeParam(5));
  aMap.Add(TypeParam(75));

  EXPECT_EQ(aMap.GetMinimalMapped(), TypeParam(5));

  aMap.Remove(TypeParam(5));
  EXPECT_EQ(aMap.GetMinimalMapped(), TypeParam(10));
}

TYPED_TEST(NCollection_PackedMapTypedTest, GetMaximalMapped)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(50));
  aMap.Add(TypeParam(10));
  aMap.Add(TypeParam(100));
  aMap.Add(TypeParam(5));
  aMap.Add(TypeParam(75));

  EXPECT_EQ(aMap.GetMaximalMapped(), TypeParam(100));

  aMap.Remove(TypeParam(100));
  EXPECT_EQ(aMap.GetMaximalMapped(), TypeParam(75));
}

//==================================================================================================
// Boolean Operations - Union
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, Union)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));
  aMap2.Add(TypeParam(5));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Union(aResult, aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 5);
  EXPECT_TRUE(aResult.Contains(TypeParam(1)));
  EXPECT_TRUE(aResult.Contains(TypeParam(2)));
  EXPECT_TRUE(aResult.Contains(TypeParam(3)));
  EXPECT_TRUE(aResult.Contains(TypeParam(4)));
  EXPECT_TRUE(aResult.Contains(TypeParam(5)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, Unite)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));
  aMap2.Add(TypeParam(5));

  EXPECT_TRUE(NCollection_PackedMapAlgo::Unite(aMap1, aMap2));

  EXPECT_EQ(aMap1.Extent(), 5);
  EXPECT_TRUE(aMap1.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(2)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(3)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(4)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(5)));

  EXPECT_FALSE(NCollection_PackedMapAlgo::Unite(aMap1, aMap2));
}

TYPED_TEST(NCollection_PackedMapTypedTest, UniteInPlace)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));

  NCollection_PackedMapAlgo::Unite(aMap1, aMap2);

  EXPECT_EQ(aMap1.Extent(), 3);
  EXPECT_TRUE(aMap1.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(2)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(3)));
}

//==================================================================================================
// Boolean Operations - Intersection
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, Intersection)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));
  aMap1.Add(TypeParam(4));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(5));
  aMap2.Add(TypeParam(6));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Intersection(aResult, aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 2);
  EXPECT_TRUE(aResult.Contains(TypeParam(2)));
  EXPECT_TRUE(aResult.Contains(TypeParam(3)));
  EXPECT_FALSE(aResult.Contains(TypeParam(1)));
  EXPECT_FALSE(aResult.Contains(TypeParam(4)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, Intersect)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));
  aMap1.Add(TypeParam(4));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(5));

  EXPECT_TRUE(NCollection_PackedMapAlgo::Intersect(aMap1, aMap2));

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(TypeParam(2)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(3)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IntersectInPlace)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));

  NCollection_PackedMapAlgo::Intersect(aMap1, aMap2);

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(TypeParam(2)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(3)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IntersectionDisjoint)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Intersection(aResult, aMap1, aMap2);

  EXPECT_TRUE(aResult.IsEmpty());
}

//==================================================================================================
// Boolean Operations - Subtraction
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, Subtraction)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));
  aMap1.Add(TypeParam(4));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(5));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Subtraction(aResult, aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 2);
  EXPECT_TRUE(aResult.Contains(TypeParam(1)));
  EXPECT_TRUE(aResult.Contains(TypeParam(4)));
  EXPECT_FALSE(aResult.Contains(TypeParam(2)));
  EXPECT_FALSE(aResult.Contains(TypeParam(3)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, Subtract)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));
  aMap1.Add(TypeParam(4));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(5));

  EXPECT_TRUE(NCollection_PackedMapAlgo::Subtract(aMap1, aMap2));

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(4)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, SubtractInPlace)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));

  NCollection_PackedMapAlgo::Subtract(aMap1, aMap2);

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(3)));
  EXPECT_FALSE(aMap1.Contains(TypeParam(2)));
}

//==================================================================================================
// Boolean Operations - Difference (XOR)
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, Difference)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Difference(aResult, aMap1, aMap2);

  EXPECT_EQ(aResult.Extent(), 2);
  EXPECT_TRUE(aResult.Contains(TypeParam(1)));
  EXPECT_TRUE(aResult.Contains(TypeParam(4)));
  EXPECT_FALSE(aResult.Contains(TypeParam(2)));
  EXPECT_FALSE(aResult.Contains(TypeParam(3)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, Differ)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));

  EXPECT_TRUE(NCollection_PackedMapAlgo::Differ(aMap1, aMap2));

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(4)));
}

TYPED_TEST(NCollection_PackedMapTypedTest, DifferInPlace)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));

  NCollection_PackedMapAlgo::Differ(aMap1, aMap2);

  EXPECT_EQ(aMap1.Extent(), 2);
  EXPECT_TRUE(aMap1.Contains(TypeParam(1)));
  EXPECT_TRUE(aMap1.Contains(TypeParam(3)));
  EXPECT_FALSE(aMap1.Contains(TypeParam(2)));
}

//==================================================================================================
// Comparison Operations
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, IsEqual)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(1));
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsEqual(aMap1, aMap2));

  aMap2.Add(TypeParam(4));
  EXPECT_FALSE(NCollection_PackedMapAlgo::IsEqual(aMap1, aMap2));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IsEqualEmpty)
{
  typename TestFixture::MapType aMap1;
  typename TestFixture::MapType aMap2;

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsEqual(aMap1, aMap2));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IsSubset)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(1));
  aMap2.Add(TypeParam(2));
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsSubset(aMap1, aMap2));
  EXPECT_FALSE(NCollection_PackedMapAlgo::IsSubset(aMap2, aMap1));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IsSubsetSame)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(1));
  aMap2.Add(TypeParam(2));

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsSubset(aMap1, aMap2));
  EXPECT_TRUE(NCollection_PackedMapAlgo::IsSubset(aMap2, aMap1));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IsSubsetEmpty)
{
  typename TestFixture::MapType aEmptyMap;
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsSubset(aEmptyMap, aMap));
  EXPECT_TRUE(NCollection_PackedMapAlgo::IsSubset(aEmptyMap, aEmptyMap));
}

TYPED_TEST(NCollection_PackedMapTypedTest, HasIntersection)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));
  aMap1.Add(TypeParam(3));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(3));
  aMap2.Add(TypeParam(4));
  aMap2.Add(TypeParam(5));

  EXPECT_TRUE(NCollection_PackedMapAlgo::HasIntersection(aMap1, aMap2));

  typename TestFixture::MapType aMap3;
  aMap3.Add(TypeParam(10));
  aMap3.Add(TypeParam(20));

  EXPECT_FALSE(NCollection_PackedMapAlgo::HasIntersection(aMap1, aMap3));
}

//==================================================================================================
// Iterator Tests
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, IteratorBasic)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(10));
  aMap.Add(TypeParam(20));
  aMap.Add(TypeParam(30));

  std::vector<TypeParam> aValues;
  for (typename TestFixture::MapType::Iterator anIt(aMap); anIt.More(); anIt.Next())
  {
    aValues.push_back(anIt.Key());
  }

  EXPECT_EQ(aValues.size(), 3);
  EXPECT_NE(std::find(aValues.begin(), aValues.end(), TypeParam(10)), aValues.end());
  EXPECT_NE(std::find(aValues.begin(), aValues.end(), TypeParam(20)), aValues.end());
  EXPECT_NE(std::find(aValues.begin(), aValues.end(), TypeParam(30)), aValues.end());
}

TYPED_TEST(NCollection_PackedMapTypedTest, IteratorEmpty)
{
  typename TestFixture::MapType aMap;

  int aCount = 0;
  for (typename TestFixture::MapType::Iterator anIt(aMap); anIt.More(); anIt.Next())
  {
    ++aCount;
  }

  EXPECT_EQ(aCount, 0);
}

TYPED_TEST(NCollection_PackedMapTypedTest, IteratorReinitialize)
{
  typename TestFixture::MapType aMap1;
  aMap1.Add(TypeParam(1));
  aMap1.Add(TypeParam(2));

  typename TestFixture::MapType aMap2;
  aMap2.Add(TypeParam(100));
  aMap2.Add(TypeParam(200));
  aMap2.Add(TypeParam(300));

  typename TestFixture::MapType::Iterator anIt(aMap1);

  int aCount1 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount1;
  }
  EXPECT_EQ(aCount1, 2);

  anIt.Initialize(aMap2);

  int aCount2 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount2;
  }
  EXPECT_EQ(aCount2, 3);
}

TYPED_TEST(NCollection_PackedMapTypedTest, IteratorReset)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));

  typename TestFixture::MapType::Iterator anIt(aMap);

  int aCount1 = 0;
  for (; anIt.More(); anIt.Next())
  {
    ++aCount1;
  }
  EXPECT_EQ(aCount1, 2);

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

TYPED_TEST(NCollection_PackedMapTypedTest, ReSize)
{
  typename TestFixture::MapType aMap(1);

  for (int i = 0; i < 1000; ++i)
  {
    aMap.Add(static_cast<TypeParam>(i));
  }

  EXPECT_EQ(aMap.Extent(), 1000);

  for (int i = 0; i < 1000; ++i)
  {
    EXPECT_TRUE(aMap.Contains(static_cast<TypeParam>(i)));
  }
}

TYPED_TEST(NCollection_PackedMapTypedTest, LargeValues)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(1000000));
  aMap.Add(TypeParam(2000000));
  aMap.Add(TypeParam(3000000));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(TypeParam(1000000)));
  EXPECT_TRUE(aMap.Contains(TypeParam(2000000)));
  EXPECT_TRUE(aMap.Contains(TypeParam(3000000)));
}

//==================================================================================================
// Self-Operations Tests
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, UnionWithSelf)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));
  aMap.Add(TypeParam(3));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Union(aResult, aMap, aMap);

  EXPECT_EQ(aResult.Extent(), 3);
  EXPECT_TRUE(NCollection_PackedMapAlgo::IsEqual(aResult, aMap));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IntersectionWithSelf)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));
  aMap.Add(TypeParam(3));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Intersection(aResult, aMap, aMap);

  EXPECT_EQ(aResult.Extent(), 3);
  EXPECT_TRUE(NCollection_PackedMapAlgo::IsEqual(aResult, aMap));
}

TYPED_TEST(NCollection_PackedMapTypedTest, SubtractionFromSelf)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));
  aMap.Add(TypeParam(3));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Subtraction(aResult, aMap, aMap);

  EXPECT_TRUE(aResult.IsEmpty());
}

TYPED_TEST(NCollection_PackedMapTypedTest, DifferenceWithSelf)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));
  aMap.Add(TypeParam(3));

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Difference(aResult, aMap, aMap);

  EXPECT_TRUE(aResult.IsEmpty());
}

//==================================================================================================
// Empty Map Operations
//==================================================================================================

TYPED_TEST(NCollection_PackedMapTypedTest, UnionWithEmpty)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));

  typename TestFixture::MapType aEmpty;

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Union(aResult, aMap, aEmpty);

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsEqual(aResult, aMap));
}

TYPED_TEST(NCollection_PackedMapTypedTest, IntersectionWithEmpty)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));

  typename TestFixture::MapType aEmpty;

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Intersection(aResult, aMap, aEmpty);

  EXPECT_TRUE(aResult.IsEmpty());
}

TYPED_TEST(NCollection_PackedMapTypedTest, SubtractionEmpty)
{
  typename TestFixture::MapType aMap;
  aMap.Add(TypeParam(1));
  aMap.Add(TypeParam(2));

  typename TestFixture::MapType aEmpty;

  typename TestFixture::MapType aResult;
  NCollection_PackedMapAlgo::Subtraction(aResult, aMap, aEmpty);

  EXPECT_TRUE(NCollection_PackedMapAlgo::IsEqual(aResult, aMap));

  typename TestFixture::MapType aResult2;
  NCollection_PackedMapAlgo::Subtraction(aResult2, aEmpty, aMap);

  EXPECT_TRUE(aResult2.IsEmpty());
}

//==================================================================================================
// Signed Integer Specific Tests (only run for signed types)
//==================================================================================================

template <typename T>
class NCollection_PackedMapSignedTest : public testing::Test
{
protected:
  using MapType = NCollection_PackedMap<T>;
};

using SignedTypes = ::testing::Types<int, long, int64_t>;

TYPED_TEST_SUITE(NCollection_PackedMapSignedTest, SignedTypes);

TYPED_TEST(NCollection_PackedMapSignedTest, NegativeValues)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(-1));
  aMap.Add(TypeParam(-100));
  aMap.Add(TypeParam(-1000));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(TypeParam(-1)));
  EXPECT_TRUE(aMap.Contains(TypeParam(-100)));
  EXPECT_TRUE(aMap.Contains(TypeParam(-1000)));
  EXPECT_FALSE(aMap.Contains(TypeParam(1)));

  EXPECT_TRUE(aMap.Remove(TypeParam(-100)));
  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_FALSE(aMap.Contains(TypeParam(-100)));
}

TYPED_TEST(NCollection_PackedMapSignedTest, MixedPositiveNegative)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(-50));
  aMap.Add(TypeParam(0));
  aMap.Add(TypeParam(50));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(TypeParam(-50)));
  EXPECT_TRUE(aMap.Contains(TypeParam(0)));
  EXPECT_TRUE(aMap.Contains(TypeParam(50)));
}

TYPED_TEST(NCollection_PackedMapSignedTest, MinMaxWithNegatives)
{
  typename TestFixture::MapType aMap;

  aMap.Add(TypeParam(-100));
  aMap.Add(TypeParam(-50));
  aMap.Add(TypeParam(0));
  aMap.Add(TypeParam(50));
  aMap.Add(TypeParam(100));

  EXPECT_EQ(aMap.GetMinimalMapped(), TypeParam(-100));
  EXPECT_EQ(aMap.GetMaximalMapped(), TypeParam(100));
}

//==================================================================================================
// 64-bit Type Specific Tests
//==================================================================================================

TEST(NCollection_PackedMap64BitTest, LargeValues64Bit)
{
  NCollection_PackedMap<int64_t> aMap;

  // Values that require 64-bit storage
  constexpr int64_t aLargeVal1 = 1LL << 40;
  constexpr int64_t aLargeVal2 = 1LL << 50;
  constexpr int64_t aLargeVal3 = 1LL << 60;

  aMap.Add(aLargeVal1);
  aMap.Add(aLargeVal2);
  aMap.Add(aLargeVal3);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(aLargeVal1));
  EXPECT_TRUE(aMap.Contains(aLargeVal2));
  EXPECT_TRUE(aMap.Contains(aLargeVal3));

  EXPECT_EQ(aMap.GetMinimalMapped(), aLargeVal1);
  EXPECT_EQ(aMap.GetMaximalMapped(), aLargeVal3);
}

TEST(NCollection_PackedMap64BitTest, Negative64BitValues)
{
  NCollection_PackedMap<int64_t> aMap;

  constexpr int64_t aNegVal1 = -(1LL << 40);
  constexpr int64_t aNegVal2 = -(1LL << 50);

  aMap.Add(aNegVal1);
  aMap.Add(aNegVal2);
  aMap.Add(int64_t(0));

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(aNegVal1));
  EXPECT_TRUE(aMap.Contains(aNegVal2));
  EXPECT_TRUE(aMap.Contains(int64_t(0)));
}

TEST(NCollection_PackedMap64BitTest, SizeTypeValues)
{
  NCollection_PackedMap<size_t> aMap;

  aMap.Add(size_t(0));
  aMap.Add(size_t(1) << 32);
  aMap.Add(std::numeric_limits<size_t>::max() / 2);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(size_t(0)));
  EXPECT_TRUE(aMap.Contains(size_t(1) << 32));
  EXPECT_TRUE(aMap.Contains(std::numeric_limits<size_t>::max() / 2));
}

//==================================================================================================
// Type Configuration Verification Tests
//==================================================================================================

TEST(NCollection_PackedMapConfigTest, ConfigFor32BitTypes)
{
  EXPECT_FALSE(NCollection_PackedMap<int>::Is64Bit);
  EXPECT_FALSE(NCollection_PackedMap<unsigned int>::Is64Bit);
  EXPECT_EQ(NCollection_PackedMap<int>::BitsPerBlock, 32);
  EXPECT_EQ(NCollection_PackedMap<unsigned int>::BitsPerBlock, 32);
}

TEST(NCollection_PackedMapConfigTest, ConfigFor64BitTypes)
{
  EXPECT_TRUE(NCollection_PackedMap<int64_t>::Is64Bit);
  EXPECT_TRUE(NCollection_PackedMap<uint64_t>::Is64Bit);
  EXPECT_TRUE(NCollection_PackedMap<size_t>::Is64Bit);
  EXPECT_EQ(NCollection_PackedMap<int64_t>::BitsPerBlock, 64);
  EXPECT_EQ(NCollection_PackedMap<size_t>::BitsPerBlock, 64);
}
