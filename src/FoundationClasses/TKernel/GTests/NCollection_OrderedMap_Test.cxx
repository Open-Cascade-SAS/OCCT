// Copyright (c) 2026 OPEN CASCADE SAS
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

#include <NCollection_OrderedMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <vector>

namespace
{
//! Collects keys from the ordered map in iteration order.
std::vector<int> collectKeys(const NCollection_OrderedMap<int>& theMap)
{
  std::vector<int> aKeys;
  for (const auto& aKey : theMap)
  {
    aKeys.push_back(aKey);
  }
  return aKeys;
}
} // namespace

TEST(NCollection_OrderedMapTest, DefaultConstructor)
{
  NCollection_OrderedMap<int> aMap;

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
}

TEST(NCollection_OrderedMapTest, ConstructorWithBuckets)
{
  const int                   nbBuckets = 100;
  NCollection_OrderedMap<int> aMap(nbBuckets);

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(nbBuckets, aMap.NbBuckets());
}

TEST(NCollection_OrderedMapTest, AddAndContainsWithOrder)
{
  NCollection_OrderedMap<int> aMap;

  EXPECT_TRUE(aMap.Add(10));
  EXPECT_TRUE(aMap.Add(20));
  EXPECT_TRUE(aMap.Add(30));

  EXPECT_EQ(3, aMap.Size());
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_FALSE(aMap.Contains(40));

  // Verify insertion order
  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, AddDuplicate)
{
  NCollection_OrderedMap<int> aMap;

  EXPECT_TRUE(aMap.Add(10));
  EXPECT_TRUE(aMap.Add(20));
  EXPECT_TRUE(aMap.Add(30));

  // Adding duplicates should return false
  EXPECT_FALSE(aMap.Add(10));
  EXPECT_FALSE(aMap.Add(20));

  // Size unchanged, order preserved
  EXPECT_EQ(3, aMap.Size());
  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, AddedMethod)
{
  NCollection_OrderedMap<int> aMap;

  const int& aRef1 = aMap.Added(10);
  EXPECT_EQ(10, aRef1);

  const int& aRef2 = aMap.Added(20);
  EXPECT_EQ(20, aRef2);

  // Added on existing returns reference to existing
  const int& aRef3 = aMap.Added(10);
  EXPECT_EQ(10, aRef3);
  EXPECT_EQ(&aRef1, &aRef3);

  EXPECT_EQ(2, aMap.Size());
}

TEST(NCollection_OrderedMapTest, RemoveFirst)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_TRUE(aMap.Remove(10));
  EXPECT_EQ(2, aMap.Size());
  EXPECT_FALSE(aMap.Contains(10));

  const std::vector<int> anExpected = {20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, RemoveMiddle)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_TRUE(aMap.Remove(20));
  EXPECT_EQ(2, aMap.Size());
  EXPECT_FALSE(aMap.Contains(20));

  const std::vector<int> anExpected = {10, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, RemoveLast)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_TRUE(aMap.Remove(30));
  EXPECT_EQ(2, aMap.Size());
  EXPECT_FALSE(aMap.Contains(30));

  const std::vector<int> anExpected = {10, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, RemoveAll)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_TRUE(aMap.Remove(20));
  EXPECT_TRUE(aMap.Remove(10));
  EXPECT_TRUE(aMap.Remove(30));

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());

  // Try removing from empty map
  EXPECT_FALSE(aMap.Remove(10));
}

TEST(NCollection_OrderedMapTest, RemoveNonExistent)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);

  EXPECT_FALSE(aMap.Remove(99));
  EXPECT_EQ(1, aMap.Size());
}

TEST(NCollection_OrderedMapTest, ClearAndReAdd)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  aMap.Clear();
  EXPECT_TRUE(aMap.IsEmpty());

  // Re-add in different order
  aMap.Add(30);
  aMap.Add(10);
  aMap.Add(20);

  const std::vector<int> anExpected = {30, 10, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, CopyPreservesOrder)
{
  NCollection_OrderedMap<int> aMap1;
  aMap1.Add(30);
  aMap1.Add(10);
  aMap1.Add(20);

  // Copy constructor
  NCollection_OrderedMap<int> aMap2(aMap1);

  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_EQ(collectKeys(aMap1), collectKeys(aMap2));

  // Modify original - copy should be independent
  aMap1.Remove(10);
  EXPECT_EQ(3, aMap2.Size());
  EXPECT_TRUE(aMap2.Contains(10));
}

TEST(NCollection_OrderedMapTest, AssignPreservesOrder)
{
  NCollection_OrderedMap<int> aMap1;
  aMap1.Add(30);
  aMap1.Add(10);
  aMap1.Add(20);

  NCollection_OrderedMap<int> aMap2;
  aMap2.Add(99); // Will be cleared by assignment
  aMap2 = aMap1;

  EXPECT_EQ(collectKeys(aMap1), collectKeys(aMap2));
  EXPECT_FALSE(aMap2.Contains(99));
}

TEST(NCollection_OrderedMapTest, MoveConstructor)
{
  NCollection_OrderedMap<int> aMap1;
  aMap1.Add(10);
  aMap1.Add(20);
  aMap1.Add(30);

  NCollection_OrderedMap<int> aMap2(std::move(aMap1));

  EXPECT_EQ(3, aMap2.Size());
  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap2));

  // Source should be empty after move
  EXPECT_TRUE(aMap1.IsEmpty());
}

TEST(NCollection_OrderedMapTest, MoveAssignment)
{
  NCollection_OrderedMap<int> aMap1;
  aMap1.Add(10);
  aMap1.Add(20);
  aMap1.Add(30);

  NCollection_OrderedMap<int> aMap2;
  aMap2 = std::move(aMap1);

  EXPECT_EQ(3, aMap2.Size());
  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap2));
}

TEST(NCollection_OrderedMapTest, Exchange)
{
  NCollection_OrderedMap<int> aMap1;
  aMap1.Add(10);
  aMap1.Add(20);

  NCollection_OrderedMap<int> aMap2;
  aMap2.Add(30);
  aMap2.Add(40);
  aMap2.Add(50);

  aMap1.Exchange(aMap2);

  EXPECT_EQ(3, aMap1.Size());
  EXPECT_EQ(2, aMap2.Size());

  const std::vector<int> anExpected1 = {30, 40, 50};
  const std::vector<int> anExpected2 = {10, 20};
  EXPECT_EQ(anExpected1, collectKeys(aMap1));
  EXPECT_EQ(anExpected2, collectKeys(aMap2));
}

TEST(NCollection_OrderedMapTest, ReSizePreservesOrder)
{
  NCollection_OrderedMap<int> aMap(10);

  for (int i = 0; i < 100; ++i)
  {
    aMap.Add(i);
  }

  EXPECT_EQ(100, aMap.Size());

  // Record order before resize
  const std::vector<int> anOrderBefore = collectKeys(aMap);

  aMap.ReSize(200);

  EXPECT_EQ(100, aMap.Size());
  EXPECT_EQ(anOrderBefore, collectKeys(aMap));

  // Verify all elements still accessible
  for (int i = 0; i < 100; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
  }
}

TEST(NCollection_OrderedMapTest, FirstLast)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(30);
  aMap.Add(10);
  aMap.Add(20);

  EXPECT_EQ(30, aMap.First());
  EXPECT_EQ(20, aMap.Last());

  // Remove first, new first should be second element
  aMap.Remove(30);
  EXPECT_EQ(10, aMap.First());
  EXPECT_EQ(20, aMap.Last());

  // Remove last
  aMap.Remove(20);
  EXPECT_EQ(10, aMap.First());
  EXPECT_EQ(10, aMap.Last());
}

TEST(NCollection_OrderedMapTest, FirstLastEmpty)
{
  NCollection_OrderedMap<int> aMap;

  EXPECT_THROW(aMap.First(), Standard_NoSuchObject);
  EXPECT_THROW(aMap.Last(), Standard_NoSuchObject);
}

TEST(NCollection_OrderedMapTest, FirstLastSingleElement)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(42);

  EXPECT_EQ(42, aMap.First());
  EXPECT_EQ(42, aMap.Last());
}

TEST(NCollection_OrderedMapTest, STLBeginEnd)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  std::vector<int> anOrder;
  for (auto it = aMap.begin(); it != aMap.end(); ++it)
  {
    anOrder.push_back(*it);
  }

  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, anOrder);
}

TEST(NCollection_OrderedMapTest, RangeBasedFor)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(100);
  aMap.Add(200);
  aMap.Add(300);

  std::vector<int> anOrder;
  for (const auto& aKey : aMap)
  {
    anOrder.push_back(aKey);
  }

  const std::vector<int> anExpected = {100, 200, 300};
  EXPECT_EQ(anExpected, anOrder);
}

TEST(NCollection_OrderedMapTest, IteratorEquality)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  EXPECT_EQ(aMap.begin(), aMap.begin());
  EXPECT_EQ(aMap.end(), aMap.end());
  EXPECT_NE(aMap.begin(), aMap.end());

  auto anIt1 = aMap.begin();
  auto anIt2 = aMap.begin();
  ++anIt2;
  EXPECT_NE(anIt1, anIt2);

  auto anIt3 = aMap.begin();
  ++anIt3;
  EXPECT_EQ(anIt2, anIt3);
}

TEST(NCollection_OrderedMapTest, LargeMapOrderPreservation)
{
  const int NUM_ELEMENTS = 1000;

  NCollection_OrderedMap<int> aMap;
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Add(i * 3); // Non-sequential to test hash distribution
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  // Verify order
  int anIdx = 0;
  for (const auto& aKey : aMap)
  {
    EXPECT_EQ(anIdx * 3, aKey);
    ++anIdx;
  }
  EXPECT_EQ(NUM_ELEMENTS, anIdx);
}

TEST(NCollection_OrderedMapTest, CustomStatefulHasher)
{
  struct StatefulHasher
  {
    int mySalt;

    StatefulHasher(int theSalt = 0)
        : mySalt(theSalt)
    {
    }

    size_t operator()(int theKey) const { return std::hash<int>{}(theKey + mySalt); }

    bool operator()(int theKey1, int theKey2) const { return theKey1 == theKey2; }
  };

  NCollection_OrderedMap<int, StatefulHasher> aMap(StatefulHasher(42), 10);

  aMap.Add(30);
  aMap.Add(10);
  aMap.Add(20);

  EXPECT_EQ(3, aMap.Size());
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_EQ(42, aMap.GetHasher().mySalt);

  // Verify order with custom hasher
  std::vector<int> anOrder;
  for (NCollection_OrderedMap<int, StatefulHasher>::Iterator anIter(aMap); anIter.More();
       anIter.Next())
  {
    anOrder.push_back(anIter.Key());
  }
  const std::vector<int> anExpected = {30, 10, 20};
  EXPECT_EQ(anExpected, anOrder);

  // Copy preserves hasher and order
  NCollection_OrderedMap<int, StatefulHasher> aMap2(aMap);
  EXPECT_EQ(42, aMap2.GetHasher().mySalt);
  std::vector<int> anOrder2;
  for (NCollection_OrderedMap<int, StatefulHasher>::Iterator anIter(aMap2); anIter.More();
       anIter.Next())
  {
    anOrder2.push_back(anIter.Key());
  }
  EXPECT_EQ(anExpected, anOrder2);
}

TEST(NCollection_OrderedMapTest, TryEmplaceBasic)
{
  NCollection_OrderedMap<TCollection_AsciiString> aMap;

  EXPECT_TRUE(aMap.TryEmplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_TRUE(aMap.Contains("Hello"));

  // TryEmplace on existing key does NOT modify
  EXPECT_FALSE(aMap.TryEmplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
}

TEST(NCollection_OrderedMapTest, TryEmplacedBasic)
{
  NCollection_OrderedMap<TCollection_AsciiString> aMap;

  const TCollection_AsciiString& aKey1 = aMap.TryEmplaced("World");
  EXPECT_TRUE(aKey1.IsEqual("World"));
  EXPECT_EQ(1, aMap.Size());

  // TryEmplaced on existing key returns reference to existing
  const TCollection_AsciiString& aKey2 = aMap.TryEmplaced("World");
  EXPECT_EQ(&aKey1, &aKey2);
}

TEST(NCollection_OrderedMapTest, TryEmplacePreservesOrder)
{
  NCollection_OrderedMap<int> aMap;
  aMap.TryEmplace(30);
  aMap.TryEmplace(10);
  aMap.TryEmplace(20);
  aMap.TryEmplace(10); // Should not change order

  const std::vector<int> anExpected = {30, 10, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, ContainedFound)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  auto aResult = aMap.Contained(20);
  EXPECT_TRUE(aResult.has_value());
  EXPECT_EQ(20, aResult->get());
}

TEST(NCollection_OrderedMapTest, ContainedNotFound)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);

  auto aResult = aMap.Contained(99);
  EXPECT_FALSE(aResult.has_value());
}

TEST(NCollection_OrderedMapTest, ContainedEmpty)
{
  NCollection_OrderedMap<int> aMap;

  auto aResult = aMap.Contained(1);
  EXPECT_FALSE(aResult.has_value());
}

TEST(NCollection_OrderedMapTest, EmplaceBasic)
{
  NCollection_OrderedMap<TCollection_AsciiString> aMap;

  EXPECT_TRUE(aMap.Emplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_TRUE(aMap.Contains("Hello"));

  // Emplace on existing key - destroys and reconstructs
  EXPECT_FALSE(aMap.Emplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
}

TEST(NCollection_OrderedMapTest, EmplacedBasic)
{
  NCollection_OrderedMap<TCollection_AsciiString> aMap;

  const TCollection_AsciiString& aKey1 = aMap.Emplaced("World");
  EXPECT_TRUE(aKey1.IsEqual("World"));
  EXPECT_EQ(1, aMap.Size());

  // Emplaced on existing key
  const TCollection_AsciiString& aKey2 = aMap.Emplaced("World");
  EXPECT_EQ(&aKey1, &aKey2);
}

TEST(NCollection_OrderedMapTest, EmplacePreservesOrder)
{
  NCollection_OrderedMap<TCollection_AsciiString> aMap;
  aMap.Emplace("Charlie");
  aMap.Emplace("Alpha");
  aMap.Emplace("Bravo");

  std::vector<TCollection_AsciiString> anOrder;
  for (const auto& aKey : aMap)
  {
    anOrder.push_back(aKey);
  }
  EXPECT_EQ(3u, anOrder.size());
  EXPECT_TRUE(anOrder[0].IsEqual("Charlie"));
  EXPECT_TRUE(anOrder[1].IsEqual("Alpha"));
  EXPECT_TRUE(anOrder[2].IsEqual("Bravo"));
}

TEST(NCollection_OrderedMapTest, NativeIterator)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  NCollection_OrderedMap<int>::Iterator anIter(aMap);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(10, anIter.Key());
  EXPECT_EQ(10, anIter.Value());

  anIter.Next();
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(20, anIter.Key());

  anIter.Next();
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(30, anIter.Key());

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST(NCollection_OrderedMapTest, IteratorInitialize)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);

  NCollection_OrderedMap<int>::Iterator anIter;
  EXPECT_FALSE(anIter.More());

  anIter.Initialize(aMap);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(10, anIter.Key());
}

TEST(NCollection_OrderedMapTest, RemoveAndReAdd)
{
  NCollection_OrderedMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  aMap.Remove(20);
  aMap.Add(40);
  aMap.Add(20); // Re-add removed element, should go at the end

  const std::vector<int> anExpected = {10, 30, 40, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST(NCollection_OrderedMapTest, AutoResize)
{
  // Start with small bucket count, let it auto-resize
  NCollection_OrderedMap<int> aMap(2);

  std::vector<int> anExpected;
  for (int i = 0; i < 50; ++i)
  {
    aMap.Add(i);
    anExpected.push_back(i);
  }

  EXPECT_EQ(50, aMap.Size());
  EXPECT_EQ(anExpected, collectKeys(aMap));
}
