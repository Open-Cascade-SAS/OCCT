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
  NCollection_Map<Standard_Integer> aMap(101);

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
  EXPECT_EQ(101, aMap.NbBuckets());
}

TEST(NCollection_MapTest, ConstructorWithBuckets)
{
  // Constructor with number of buckets
  const Standard_Integer            nbBuckets = 100;
  NCollection_Map<Standard_Integer> aMap(nbBuckets);

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
  EXPECT_EQ(nbBuckets, aMap.NbBuckets());
}

TEST(NCollection_MapTest, AddAndContains)
{
  NCollection_Map<Standard_Integer> aMap;

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
  NCollection_Map<Standard_Integer> aMap;
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
  NCollection_Map<Standard_Integer> aMap;
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
  NCollection_Map<Standard_Integer> aMap1;
  aMap1.Add(10);
  aMap1.Add(20);
  aMap1.Add(30);

  // Test assignment operator
  NCollection_Map<Standard_Integer> aMap2;
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
  NCollection_Map<Standard_Integer> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Test iteration using OCCT iterator
  NCollection_Map<Standard_Integer>::Iterator it(aMap);

  // Create set to check all keys are visited
  std::set<Standard_Integer> foundKeys;

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
  NCollection_Map<Standard_Integer> aMap(10);

  // Add elements
  for (Standard_Integer i = 0; i < 100; ++i)
  {
    aMap.Add(i);
  }

  // Check initial state
  EXPECT_EQ(100, aMap.Size());

  // Before resize, remember which elements are contained
  std::vector<Standard_Integer> elements;
  for (NCollection_Map<Standard_Integer>::Iterator it(aMap); it.More(); it.Next())
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
  NCollection_Map<Standard_Integer> aMap;

  // Add many elements
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Add(i);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  // Count elements using iterator
  int                                         count = 0;
  int                                         sum   = 0;
  NCollection_Map<Standard_Integer>::Iterator it(aMap);
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
  const Standard_Integer aLeftLower  = 1;
  const Standard_Integer aLeftUpper  = 10;
  const Standard_Integer aRightLower = 5;
  const Standard_Integer aRightUpper = 15;

  NCollection_Map<Standard_Integer> aMapLeft;
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    aMapLeft.Add(aKeyIter);
  }

  NCollection_Map<Standard_Integer> aMapRight;
  for (Standard_Integer aKeyIter = aRightLower; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    aMapRight.Add(aKeyIter);
  }

  EXPECT_FALSE(NCollection_MapAlgo::Contains(aMapLeft, aMapRight));
  EXPECT_FALSE(NCollection_MapAlgo::Contains(aMapRight, aMapLeft));

  NCollection_Map<Standard_Integer> aMapUnion;
  NCollection_MapAlgo::Union(aMapUnion, aMapLeft, aMapRight);
  EXPECT_EQ(aRightUpper - aLeftLower + 1, aMapUnion.Extent());
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapUnion.Contains(aKeyIter));
  }

  NCollection_Map<Standard_Integer> aMapSect;
  NCollection_MapAlgo::Intersection(aMapSect, aMapLeft, aMapRight);
  EXPECT_EQ(aLeftUpper - aRightLower + 1, aMapSect.Extent());
  for (Standard_Integer aKeyIter = aRightLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSect.Contains(aKeyIter));
  }
  EXPECT_TRUE(NCollection_MapAlgo::Contains(aMapLeft, aMapSect));
  EXPECT_TRUE(NCollection_MapAlgo::Contains(aMapRight, aMapSect));

  NCollection_Map<Standard_Integer> aMapSubsLR;
  NCollection_MapAlgo::Subtraction(aMapSubsLR, aMapLeft, aMapRight);
  EXPECT_EQ(aRightLower - aLeftLower, aMapSubsLR.Extent());
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter < aRightLower; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSubsLR.Contains(aKeyIter));
  }

  NCollection_Map<Standard_Integer> aMapSubsRL;
  NCollection_MapAlgo::Subtraction(aMapSubsRL, aMapRight, aMapLeft);
  EXPECT_EQ(aRightUpper - aLeftUpper, aMapSubsRL.Extent());
  for (Standard_Integer aKeyIter = aLeftUpper + 1; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSubsRL.Contains(aKeyIter));
  }

  NCollection_Map<Standard_Integer> aMapDiff;
  NCollection_MapAlgo::Difference(aMapDiff, aMapLeft, aMapRight);
  EXPECT_EQ(aRightLower - aLeftLower + aRightUpper - aLeftUpper, aMapDiff.Extent());
  for (Standard_Integer aKeyIter = aLeftLower; aKeyIter < aRightLower; ++aKeyIter)
  {
    EXPECT_TRUE(aMapDiff.Contains(aKeyIter));
  }
  for (Standard_Integer aKeyIter = aLeftUpper + 1; aKeyIter <= aRightUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapDiff.Contains(aKeyIter));
  }

  NCollection_Map<Standard_Integer> aMapSwap;
  aMapSwap.Exchange(aMapSect);
  for (Standard_Integer aKeyIter = aRightLower; aKeyIter <= aLeftUpper; ++aKeyIter)
  {
    EXPECT_TRUE(aMapSwap.Contains(aKeyIter));
  }
  EXPECT_TRUE(aMapSect.IsEmpty());
}
