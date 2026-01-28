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

#include <NCollection_Map.hxx>
#include <NCollection_MapAlgo.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

TEST(NCollection_MapTest, DefaultConstructor)
{
  // Default constructor should create an empty map
  NCollection_Map<int> aMap(101);

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
  EXPECT_EQ(101, aMap.NbBuckets());
}

TEST(NCollection_MapTest, ConstructorWithBuckets)
{
  // Constructor with number of buckets
  const int            nbBuckets = 100;
  NCollection_Map<int> aMap(nbBuckets);

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
  EXPECT_EQ(nbBuckets, aMap.NbBuckets());
}

TEST(NCollection_MapTest, AddAndContains)
{
  NCollection_Map<int> aMap;

  // Test Add method
  EXPECT_TRUE(aMap.Add(10));
  EXPECT_TRUE(aMap.Add(20));
  EXPECT_TRUE(aMap.Add(30));

  // Adding duplicates should return false
  EXPECT_FALSE(aMap.Add(10));
  EXPECT_FALSE(aMap.Add(20));

  // Map size should account for unique elements only
  EXPECT_EQ(3, aMap.Size());

  // Test Contains
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_FALSE(aMap.Contains(40));
}

TEST(NCollection_MapTest, Remove)
{
  NCollection_Map<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Test Remove
  EXPECT_TRUE(aMap.Remove(20));
  EXPECT_EQ(2, aMap.Size());
  EXPECT_FALSE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(30));

  // Try to remove non-existent element
  EXPECT_FALSE(aMap.Remove(40));
  EXPECT_EQ(2, aMap.Size());
}

TEST(NCollection_MapTest, Clear)
{
  NCollection_Map<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_FALSE(aMap.IsEmpty());

  // Test Clear
  aMap.Clear();
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_FALSE(aMap.Contains(10));
  EXPECT_FALSE(aMap.Contains(20));
  EXPECT_FALSE(aMap.Contains(30));
}

TEST(NCollection_MapTest, Assignment)
{
  NCollection_Map<int> aMap1;
  aMap1.Add(10);
  aMap1.Add(20);
  aMap1.Add(30);

  // Test assignment operator
  NCollection_Map<int> aMap2;
  aMap2 = aMap1;

  // Check both maps have the same content
  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
  EXPECT_TRUE(aMap2.Contains(30));

  // Modify original map to ensure deep copy
  aMap1.Add(40);
  aMap1.Remove(10);

  EXPECT_EQ(3, aMap1.Size());
  EXPECT_EQ(3, aMap2.Size());
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_FALSE(aMap1.Contains(10));
  EXPECT_TRUE(aMap1.Contains(40));
  EXPECT_FALSE(aMap2.Contains(40));
}

TEST(NCollection_MapTest, IteratorAccess)
{
  NCollection_Map<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Test iteration using OCCT iterator
  NCollection_Map<int>::Iterator it(aMap);

  // Create set to check all keys are visited
  std::set<int> foundKeys;

  for (; it.More(); it.Next())
  {
    foundKeys.insert(it.Value());
  }

  // Check all keys were visited
  EXPECT_EQ(3, foundKeys.size());
  EXPECT_TRUE(foundKeys.find(10) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(20) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(30) != foundKeys.end());
}

TEST(NCollection_MapTest, Resize)
{
  NCollection_Map<int> aMap(10);

  // Add elements
  for (int i = 0; i < 100; ++i)
  {
    aMap.Add(i);
  }

  // Check initial state
  EXPECT_EQ(100, aMap.Size());

  // Before resize, remember which elements are contained
  std::vector<int> elements;
  for (NCollection_Map<int>::Iterator it(aMap); it.More(); it.Next())
  {
    elements.push_back(it.Value());
  }

  // Test Resize
  aMap.ReSize(200);

  // Resize shouldn't change the map contents
  EXPECT_EQ(100, aMap.Size());
  for (const auto& element : elements)
  {
    EXPECT_TRUE(aMap.Contains(element));
  }
}

TEST(NCollection_MapTest, ExhaustiveIterator)
{
  const int NUM_ELEMENTS = 1000;

  // Create a map with many elements to test iterator efficiency
  NCollection_Map<int> aMap;

  // Add many elements
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Add(i);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  // Count elements using iterator
  int                            count = 0;
  int                            sum   = 0;
  NCollection_Map<int>::Iterator it(aMap);
  for (; it.More(); it.Next())
  {
    sum += it.Value();
    count++;
  }

  EXPECT_EQ(NUM_ELEMENTS, count);

  // Calculate expected sum: 0 + 1 + 2 + ... + (NUM_ELEMENTS-1)
  int expectedSum = (NUM_ELEMENTS * (NUM_ELEMENTS - 1)) / 2;
  EXPECT_EQ(expectedSum, sum);
}

TEST(NCollection_MapTest, OCC24271_BooleanOperations)
{
  const int aLeftLower  = 1;
  const int aLeftUpper  = 10;
  const int aRightLower = 5;
  const int aRightUpper = 15;

  NCollection_Map<int> aMapLeft;
  for (int aKeyIter = aLeftLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    aMapLeft.Add(aKeyIter);
  }

  NCollection_Map<int> aMapRight;
  for (int aKeyIter = aRightLower; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    aMapRight.Add(aKeyIter);
  }

  EXPECT_FALSE(NCollection_MapAlgo::Contains(aMapLeft, aMapRight));
  EXPECT_FALSE(NCollection_MapAlgo::Contains(aMapRight, aMapLeft));

  NCollection_Map<int> aMapUnion;
  NCollection_MapAlgo::Union(aMapUnion, aMapLeft, aMapRight);
  EXPECT_EQ(aRightUpper - aLeftLower + 1, aMapUnion.Extent());
  for (int aKeyIter = aLeftLower; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapUnion.Contains(aKeyIter));
  }

  NCollection_Map<int> aMapSect;
  NCollection_MapAlgo::Intersection(aMapSect, aMapLeft, aMapRight);
  EXPECT_EQ(aLeftUpper - aRightLower + 1, aMapSect.Extent());
  for (int aKeyIter = aRightLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSect.Contains(aKeyIter));
  }
  EXPECT_TRUE(NCollection_MapAlgo::Contains(aMapLeft, aMapSect));
  EXPECT_TRUE(NCollection_MapAlgo::Contains(aMapRight, aMapSect));

  NCollection_Map<int> aMapSubsLR;
  NCollection_MapAlgo::Subtraction(aMapSubsLR, aMapLeft, aMapRight);
  EXPECT_EQ(aRightLower - aLeftLower, aMapSubsLR.Extent());
  for (int aKeyIter = aLeftLower; aKeyIter < aRightLower; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSubsLR.Contains(aKeyIter));
  }

  NCollection_Map<int> aMapSubsRL;
  NCollection_MapAlgo::Subtraction(aMapSubsRL, aMapRight, aMapLeft);
  EXPECT_EQ(aRightUpper - aLeftUpper, aMapSubsRL.Extent());
  for (int aKeyIter = aLeftUpper + 1; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSubsRL.Contains(aKeyIter));
  }

  NCollection_Map<int> aMapDiff;
  NCollection_MapAlgo::Difference(aMapDiff, aMapLeft, aMapRight);
  EXPECT_EQ(aRightLower - aLeftLower + aRightUpper - aLeftUpper, aMapDiff.Extent());
  for (int aKeyIter = aLeftLower; aKeyIter < aRightLower; ++aKeyIter)
  {
    EXPECT_TRUE(aMapDiff.Contains(aKeyIter));
  }
  for (int aKeyIter = aLeftUpper + 1; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapDiff.Contains(aKeyIter));
  }

  NCollection_Map<int> aMapSwap;
  aMapSwap.Exchange(aMapSect);
  for (int aKeyIter = aRightLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSwap.Contains(aKeyIter));
  }
  EXPECT_TRUE(aMapSect.IsEmpty());
}

// Tests for Emplace methods
TEST(NCollection_MapTest, EmplaceBasic)
{
  NCollection_Map<TCollection_AsciiString> aMap;

  // Test Emplace - should return true for new key
  EXPECT_TRUE(aMap.Emplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_TRUE(aMap.Contains("Hello"));

  // Test Emplace on existing key - destroys and reconstructs
  EXPECT_FALSE(aMap.Emplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
}

TEST(NCollection_MapTest, EmplacedBasic)
{
  NCollection_Map<TCollection_AsciiString> aMap;

  // Test Emplaced - should return reference to key
  const TCollection_AsciiString& aKey1 = aMap.Emplaced("World");
  EXPECT_TRUE(aKey1.IsEqual("World"));
  EXPECT_EQ(1, aMap.Size());

  // Test Emplaced on existing key - should return existing
  const TCollection_AsciiString& aKey2 = aMap.Emplaced("World");
  EXPECT_EQ(&aKey1, &aKey2); // Same reference (after reconstruction)
}

// Tests for hasher constructor
TEST(NCollection_MapTest, HasherConstructorCopy)
{
  // Custom hasher with state
  struct StatefulHasher
  {
    int mySalt;
    StatefulHasher(int theSalt = 0)
        : mySalt(theSalt)
    {
    }
    size_t operator()(int theKey) const { return std::hash<int>{}(theKey + mySalt); }
    bool   operator()(int theKey1, int theKey2) const { return theKey1 == theKey2; }
  };

  StatefulHasher                         aHasher(42);
  NCollection_Map<int, StatefulHasher> aMap(aHasher, 10);

  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  EXPECT_EQ(3, aMap.Size());
  EXPECT_TRUE(aMap.Contains(1));
  EXPECT_TRUE(aMap.Contains(2));
  EXPECT_TRUE(aMap.Contains(3));

  // Verify hasher was copied
  const StatefulHasher& aMapHasher = aMap.GetHasher();
  EXPECT_EQ(42, aMapHasher.mySalt);
}

TEST(NCollection_MapTest, HasherConstructorMove)
{
  struct StatefulHasher
  {
    int mySalt;
    StatefulHasher(int theSalt = 0)
        : mySalt(theSalt)
    {
    }
    size_t operator()(int theKey) const { return std::hash<int>{}(theKey + mySalt); }
    bool   operator()(int theKey1, int theKey2) const { return theKey1 == theKey2; }
  };

  NCollection_Map<int, StatefulHasher> aMap(StatefulHasher(99), 10);

  aMap.Add(10);
  aMap.Add(20);

  EXPECT_EQ(2, aMap.Size());
  EXPECT_EQ(99, aMap.GetHasher().mySalt);
}

TEST(NCollection_MapTest, CopyConstructorPreservesHasher)
{
  struct StatefulHasher
  {
    int mySalt;
    StatefulHasher(int theSalt = 0)
        : mySalt(theSalt)
    {
    }
    size_t operator()(int theKey) const { return std::hash<int>{}(theKey + mySalt); }
    bool   operator()(int theKey1, int theKey2) const { return theKey1 == theKey2; }
  };

  NCollection_Map<int, StatefulHasher> aMap1(StatefulHasher(123), 10);
  aMap1.Add(1);
  aMap1.Add(2);

  // Copy construct
  NCollection_Map<int, StatefulHasher> aMap2(aMap1);

  EXPECT_EQ(123, aMap2.GetHasher().mySalt);
  EXPECT_EQ(2, aMap2.Size());
  EXPECT_TRUE(aMap2.Contains(1));
  EXPECT_TRUE(aMap2.Contains(2));
}

// Tests for begin()/end() iterator
TEST(NCollection_MapTest, BeginEndIterator)
{
  NCollection_Map<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Test begin/end iteration
  std::set<int> aFoundKeys;
  for (auto it = aMap.begin(); it != aMap.end(); ++it)
  {
    aFoundKeys.insert(*it);
  }

  EXPECT_EQ(3u, aFoundKeys.size());
  EXPECT_TRUE(aFoundKeys.count(10) > 0);
  EXPECT_TRUE(aFoundKeys.count(20) > 0);
  EXPECT_TRUE(aFoundKeys.count(30) > 0);
}

TEST(NCollection_MapTest, RangeBasedForLoop)
{
  NCollection_Map<int> aMap;
  aMap.Add(100);
  aMap.Add(200);
  aMap.Add(300);

  // Test range-based for loop
  std::set<int> aFoundKeys;
  for (const auto& aKey : aMap)
  {
    aFoundKeys.insert(aKey);
  }

  EXPECT_EQ(3u, aFoundKeys.size());
  EXPECT_TRUE(aFoundKeys.count(100) > 0);
  EXPECT_TRUE(aFoundKeys.count(200) > 0);
  EXPECT_TRUE(aFoundKeys.count(300) > 0);
}

// Test iterator equality using NCollection_StlIterator
TEST(NCollection_MapTest, IteratorEquality)
{
  NCollection_Map<int> aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  // begin() should equal begin()
  EXPECT_EQ(aMap.begin(), aMap.begin());

  // end() should equal end()
  EXPECT_EQ(aMap.end(), aMap.end());

  // begin() should not equal end() when map is not empty
  EXPECT_NE(aMap.begin(), aMap.end());

  // Two iterators at different positions must NOT be equal
  auto anIt1 = aMap.begin();
  auto anIt2 = aMap.begin();
  ++anIt2;
  EXPECT_NE(anIt1, anIt2);

  // Iterators at same position should be equal
  auto anIt3 = aMap.begin();
  ++anIt3;
  EXPECT_EQ(anIt2, anIt3);
}
