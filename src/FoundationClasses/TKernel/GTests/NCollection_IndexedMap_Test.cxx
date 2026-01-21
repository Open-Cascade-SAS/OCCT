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

#include <NCollection_IndexedMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <random>
#include <vector>

// Basic test type for the IndexedMap
typedef int KeyType;

// Custom class for testing complex keys
class TestKey
{
public:
  TestKey(int id = 0, const char* name = "")
      : myId(id),
        myName(name)
  {
  }

  bool operator==(const TestKey& other) const
  {
    return myId == other.myId && myName == other.myName;
  }

  int GetId() const { return myId; }

  const std::string& GetName() const { return myName; }

private:
  int         myId;
  std::string myName;
};

// Hasher for TestKey
struct TestKeyHasher
{
  size_t operator()(const TestKey& theKey) const
  {
    // Combine the hash of the id and name
    size_t aCombination[2] = {std::hash<int>()(theKey.GetId()),
                              std::hash<std::string>()(theKey.GetName())};

    return opencascade::hashBytes(aCombination, sizeof(aCombination));
  }

  bool operator()(const TestKey& theKey1, const TestKey& theKey2) const
  {
    return theKey1 == theKey2;
  }
};

TEST(NCollection_IndexedMapTest, DefaultConstructor)
{
  // Test default constructor
  NCollection_IndexedMap<KeyType> aMap;
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
  EXPECT_EQ(aMap.Size(), 0);
}

TEST(NCollection_IndexedMapTest, BasicAddFind)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Test adding elements
  int index1 = aMap.Add(10);
  int index2 = aMap.Add(20);
  int index3 = aMap.Add(30);

  EXPECT_EQ(index1, 1);
  EXPECT_EQ(index2, 2);
  EXPECT_EQ(index3, 3);
  EXPECT_EQ(aMap.Extent(), 3);

  // Test finding elements by key
  EXPECT_EQ(aMap.FindIndex(10), 1);
  EXPECT_EQ(aMap.FindIndex(20), 2);
  EXPECT_EQ(aMap.FindIndex(30), 3);

  // Test finding keys by index
  EXPECT_EQ(aMap.FindKey(1), 10);
  EXPECT_EQ(aMap.FindKey(2), 20);
  EXPECT_EQ(aMap.FindKey(3), 30);

  // Test operator()
  EXPECT_EQ(aMap(1), 10);
  EXPECT_EQ(aMap(2), 20);
  EXPECT_EQ(aMap(3), 30);

  // Test Contains
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_FALSE(aMap.Contains(40));
}

TEST(NCollection_IndexedMapTest, DuplicateKey)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add initial elements
  int index1 = aMap.Add(10);
  int index2 = aMap.Add(20);

  EXPECT_EQ(index1, 1);
  EXPECT_EQ(index2, 2);

  // Try to add a duplicate - should return the existing index
  int indexDup = aMap.Add(10);
  EXPECT_EQ(indexDup, 1);
  EXPECT_EQ(aMap.Extent(), 2); // Size should not change
}

TEST(NCollection_IndexedMapTest, RemoveLast)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Remove the last element
  aMap.RemoveLast();

  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_FALSE(aMap.Contains(30));

  // Check indices - they should still be sequential
  EXPECT_EQ(aMap.FindIndex(10), 1);
  EXPECT_EQ(aMap.FindIndex(20), 2);
}

TEST(NCollection_IndexedMapTest, RemoveFromIndex)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);
  aMap.Add(40);

  // Remove element at index 2
  aMap.RemoveFromIndex(2);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_FALSE(aMap.Contains(20)); // Removed
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_TRUE(aMap.Contains(40));

  // Test how the indices were rearranged
  // Index 2 should now contain what was previously at the last position
  EXPECT_EQ(aMap.FindKey(2), 40); // Last element moved to position 2
  EXPECT_EQ(aMap.FindKey(1), 10); // First element unchanged
  EXPECT_EQ(aMap.FindKey(3), 30); // Third element unchanged
}

TEST(NCollection_IndexedMapTest, RemoveKey)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Remove by key
  bool removed = aMap.RemoveKey(20);

  EXPECT_TRUE(removed);
  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_FALSE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));

  // Try to remove non-existent key
  removed = aMap.RemoveKey(50);
  EXPECT_FALSE(removed);
  EXPECT_EQ(aMap.Extent(), 2);
}

TEST(NCollection_IndexedMapTest, Substitute)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Substitute key at index 2
  aMap.Substitute(2, 25);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_FALSE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(25));
  EXPECT_TRUE(aMap.Contains(30));

  // Check indices after substitution
  EXPECT_EQ(aMap.FindIndex(10), 1);
  EXPECT_EQ(aMap.FindIndex(25), 2);
  EXPECT_EQ(aMap.FindIndex(30), 3);

  // Check keys
  EXPECT_EQ(aMap.FindKey(1), 10);
  EXPECT_EQ(aMap.FindKey(2), 25);
  EXPECT_EQ(aMap.FindKey(3), 30);
}

TEST(NCollection_IndexedMapTest, Swap)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Swap elements at indices 1 and 3
  aMap.Swap(1, 3);

  EXPECT_EQ(aMap.Extent(), 3);

  // Check that keys maintained their values
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));

  // But indices are swapped
  EXPECT_EQ(aMap.FindKey(1), 30);
  EXPECT_EQ(aMap.FindKey(2), 20);
  EXPECT_EQ(aMap.FindKey(3), 10);

  EXPECT_EQ(aMap.FindIndex(10), 3);
  EXPECT_EQ(aMap.FindIndex(20), 2);
  EXPECT_EQ(aMap.FindIndex(30), 1);
}

TEST(NCollection_IndexedMapTest, Clear)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  EXPECT_EQ(aMap.Extent(), 3);

  // Clear the map
  aMap.Clear();

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);

  // Ensure we can add elements again
  int index1 = aMap.Add(40);
  EXPECT_EQ(index1, 1);
  EXPECT_EQ(aMap.Extent(), 1);
}

TEST(NCollection_IndexedMapTest, CopyConstructor)
{
  NCollection_IndexedMap<KeyType> aMap1;

  // Add elements to first map
  aMap1.Add(10);
  aMap1.Add(20);
  aMap1.Add(30);

  // Create a copy
  NCollection_IndexedMap<KeyType> aMap2(aMap1);

  // Check that copy has the same elements
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
  EXPECT_TRUE(aMap2.Contains(30));

  // Check indices
  EXPECT_EQ(aMap2.FindIndex(10), 1);
  EXPECT_EQ(aMap2.FindIndex(20), 2);
  EXPECT_EQ(aMap2.FindIndex(30), 3);

  // Modify original to ensure deep copy
  aMap1.Add(40);
  EXPECT_EQ(aMap1.Extent(), 4);
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_FALSE(aMap2.Contains(40));
}

TEST(NCollection_IndexedMapTest, AssignmentOperator)
{
  NCollection_IndexedMap<KeyType> aMap1;
  NCollection_IndexedMap<KeyType> aMap2;

  // Add elements to first map
  aMap1.Add(10);
  aMap1.Add(20);

  // Add different elements to second map
  aMap2.Add(30);
  aMap2.Add(40);
  aMap2.Add(50);

  // Assign first to second
  aMap2 = aMap1;

  // Check that second map now matches first
  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
  EXPECT_FALSE(aMap2.Contains(30));
  EXPECT_FALSE(aMap2.Contains(40));
  EXPECT_FALSE(aMap2.Contains(50));
}

TEST(NCollection_IndexedMapTest, Iterator)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Use iterator to check all elements
  bool   found10 = false;
  bool   found20 = false;
  bool   found30 = false;
  size_t count   = 0;

  for (NCollection_IndexedMap<KeyType>::Iterator it(aMap); it.More(); it.Next(), ++count)
  {
    const KeyType& key = it.Value();
    if (key == 10)
      found10 = true;
    else if (key == 20)
      found20 = true;
    else if (key == 30)
      found30 = true;
  }

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(found10);
  EXPECT_TRUE(found20);
  EXPECT_TRUE(found30);
}

TEST(NCollection_IndexedMapTest, StlIterator)
{
  NCollection_IndexedMap<KeyType> aMap;

  // Add elements
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  // Use STL-style iterator
  bool   found10 = false;
  bool   found20 = false;
  bool   found30 = false;
  size_t count   = 0;

  for (auto it = aMap.cbegin(); it != aMap.cend(); ++it, ++count)
  {
    if (*it == 10)
      found10 = true;
    else if (*it == 20)
      found20 = true;
    else if (*it == 30)
      found30 = true;
  }

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(found10);
  EXPECT_TRUE(found20);
  EXPECT_TRUE(found30);
}

TEST(NCollection_IndexedMapTest, StringKeys)
{
  // Test with string keys
  NCollection_IndexedMap<TCollection_AsciiString> aStringMap;

  // Add string keys
  int index1 = aStringMap.Add(TCollection_AsciiString("First"));
  int index2 = aStringMap.Add(TCollection_AsciiString("Second"));
  int index3 = aStringMap.Add(TCollection_AsciiString("Third"));

  EXPECT_EQ(index1, 1);
  EXPECT_EQ(index2, 2);
  EXPECT_EQ(index3, 3);

  // Find by key
  EXPECT_EQ(aStringMap.FindIndex(TCollection_AsciiString("First")), 1);
  EXPECT_EQ(aStringMap.FindIndex(TCollection_AsciiString("Second")), 2);
  EXPECT_EQ(aStringMap.FindIndex(TCollection_AsciiString("Third")), 3);

  // Find by index
  EXPECT_TRUE(aStringMap.FindKey(1).IsEqual("First"));
  EXPECT_TRUE(aStringMap.FindKey(2).IsEqual("Second"));
  EXPECT_TRUE(aStringMap.FindKey(3).IsEqual("Third"));
}

TEST(NCollection_IndexedMapTest, ComplexKeys)
{
  // Create map with custom key and hasher
  NCollection_IndexedMap<TestKey, TestKeyHasher> aComplexMap;

  // Add complex keys
  TestKey key1(1, "One");
  TestKey key2(2, "Two");
  TestKey key3(3, "Three");

  int index1 = aComplexMap.Add(key1);
  int index2 = aComplexMap.Add(key2);
  int index3 = aComplexMap.Add(key3);

  EXPECT_EQ(index1, 1);
  EXPECT_EQ(index2, 2);
  EXPECT_EQ(index3, 3);

  // Find by key
  EXPECT_EQ(aComplexMap.FindIndex(key1), 1);
  EXPECT_EQ(aComplexMap.FindIndex(key2), 2);
  EXPECT_EQ(aComplexMap.FindIndex(key3), 3);

  // Find by index
  EXPECT_EQ(aComplexMap.FindKey(1).GetId(), 1);
  EXPECT_EQ(aComplexMap.FindKey(2).GetId(), 2);
  EXPECT_EQ(aComplexMap.FindKey(3).GetId(), 3);

  // Test contains
  EXPECT_TRUE(aComplexMap.Contains(TestKey(1, "One")));
  EXPECT_FALSE(aComplexMap.Contains(TestKey(4, "Four")));
}

TEST(NCollection_IndexedMapTest, Exchange)
{
  NCollection_IndexedMap<KeyType> aMap1;
  NCollection_IndexedMap<KeyType> aMap2;

  // Add elements to first map
  aMap1.Add(10);
  aMap1.Add(20);

  // Add different elements to second map
  aMap2.Add(30);
  aMap2.Add(40);
  aMap2.Add(50);

  // Exchange maps
  aMap1.Exchange(aMap2);

  // Check that maps are exchanged
  EXPECT_EQ(aMap1.Extent(), 3);
  EXPECT_TRUE(aMap1.Contains(30));
  EXPECT_TRUE(aMap1.Contains(40));
  EXPECT_TRUE(aMap1.Contains(50));

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
}

TEST(NCollection_IndexedMapTest, ReSize)
{
  NCollection_IndexedMap<KeyType> aMap(3); // Start with small bucket count

  // Add many elements to trigger resize
  for (int i = 1; i <= 100; ++i)
  {
    aMap.Add(i);
  }

  // Verify all elements are present
  EXPECT_EQ(aMap.Extent(), 100);
  for (int i = 1; i <= 100; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
    EXPECT_EQ(aMap.FindIndex(i), i);
    EXPECT_EQ(aMap.FindKey(i), i);
  }

  // Explicitly resize
  aMap.ReSize(200);

  // Check that elements are still accessible
  EXPECT_EQ(aMap.Extent(), 100);
  for (int i = 1; i <= 100; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
    EXPECT_EQ(aMap.FindIndex(i), i);
    EXPECT_EQ(aMap.FindKey(i), i);
  }
}

TEST(NCollection_IndexedMapTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_IndexedMap<int> aMap;
  std::vector<int>            aVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
    aMap.Add(aVal);
    aVector.push_back(aVal);
  }

  auto aMinOCCT = std::min_element(aMap.cbegin(), aMap.cend());
  auto aMinStd  = std::min_element(aVector.begin(), aVector.end());

  auto aMaxOCCT = std::max_element(aMap.cbegin(), aMap.cend());
  auto aMaxStd  = std::max_element(aVector.begin(), aVector.end());

  EXPECT_EQ(*aMinOCCT, *aMinStd);
  EXPECT_EQ(*aMaxOCCT, *aMaxStd);
}

TEST(NCollection_IndexedMapTest, STLAlgorithmCompatibility_Find)
{
  NCollection_IndexedMap<int> aMap;
  std::vector<int>            aVector;

  std::mt19937                       aGenerator(1); // Fixed seed for reproducible tests
  std::uniform_int_distribution<int> aDistribution(0, RAND_MAX);
  for (int anIdx = 0; anIdx < 100; ++anIdx)
  {
    int aVal = aDistribution(aGenerator);
    aMap.Add(aVal);
    aVector.push_back(aVal);
  }

  // Test std::find compatibility
  int  aSearchValue = aVector[10];
  auto aFoundOCCT   = std::find(aMap.cbegin(), aMap.cend(), aSearchValue);
  auto aFoundStd    = std::find(aVector.begin(), aVector.end(), aSearchValue);

  EXPECT_TRUE(aFoundOCCT != aMap.cend());
  EXPECT_TRUE(aFoundStd != aVector.end());
  EXPECT_EQ(*aFoundOCCT, *aFoundStd);
}

TEST(NCollection_IndexedMapTest, Added_NewKey)
{
  NCollection_IndexedMap<int> aMap;

  // Added on new key should add and return reference to the key in the map
  const int& aRef1 = aMap.Added(10);
  EXPECT_EQ(10, aRef1);
  EXPECT_EQ(1, aMap.Extent());
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_EQ(1, aMap.FindIndex(10));

  // Add another key
  const int& aRef2 = aMap.Added(20);
  EXPECT_EQ(20, aRef2);
  EXPECT_EQ(2, aMap.Extent());
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_EQ(2, aMap.FindIndex(20));
}

TEST(NCollection_IndexedMapTest, Added_ExistingKey)
{
  NCollection_IndexedMap<int> aMap;

  // First add a key
  aMap.Add(10);
  EXPECT_EQ(1, aMap.Extent());

  // Added on existing key should return reference to existing key (not add duplicate)
  const int& aRef = aMap.Added(10);
  EXPECT_EQ(10, aRef);
  EXPECT_EQ(1, aMap.Extent()); // Size should not change
  EXPECT_EQ(1, aMap.FindIndex(10)); // Index should be the same
}

TEST(NCollection_IndexedMapTest, Added_MoveSemantics)
{
  NCollection_IndexedMap<TCollection_AsciiString> aMap;

  // Test with rvalue
  const TCollection_AsciiString& aRef1 = aMap.Added(TCollection_AsciiString("First"));
  EXPECT_TRUE(aRef1.IsEqual("First"));
  EXPECT_EQ(1, aMap.Extent());

  // Test that existing key returns same reference (no duplicate)
  const TCollection_AsciiString& aRef2 = aMap.Added(TCollection_AsciiString("First"));
  EXPECT_TRUE(aRef2.IsEqual("First"));
  EXPECT_EQ(1, aMap.Extent()); // Size should not change
}

TEST(NCollection_IndexedMapTest, Added_ReferenceValidity)
{
  NCollection_IndexedMap<int> aMap;

  // Add multiple keys and verify references point to map contents
  const int& aRef1 = aMap.Added(10);
  const int& aRef2 = aMap.Added(20);
  const int& aRef3 = aMap.Added(30);

  // Verify values
  EXPECT_EQ(10, aRef1);
  EXPECT_EQ(20, aRef2);
  EXPECT_EQ(30, aRef3);

  // Verify references point to actual map keys
  EXPECT_EQ(&aRef1, &aMap.FindKey(1));
  EXPECT_EQ(&aRef2, &aMap.FindKey(2));
  EXPECT_EQ(&aRef3, &aMap.FindKey(3));
}

// Tests for Emplace methods
TEST(NCollection_IndexedMapTest, Emplace_NewKey)
{
  NCollection_IndexedMap<TCollection_AsciiString> aMap;

  // Emplace with new key should return index
  int aIdx = aMap.Emplace("First");
  EXPECT_EQ(1, aIdx);
  EXPECT_EQ(1, aMap.Extent());
  EXPECT_TRUE(aMap.FindKey(1).IsEqual("First"));
}

TEST(NCollection_IndexedMapTest, Emplace_ExistingKey)
{
  NCollection_IndexedMap<TCollection_AsciiString> aMap;
  aMap.Add("First");

  // Emplace on existing key should destroy+reconstruct and return existing index
  int aIdx = aMap.Emplace("First");
  EXPECT_EQ(1, aIdx);
  EXPECT_EQ(1, aMap.Extent());
}

TEST(NCollection_IndexedMapTest, Emplaced_NewKey)
{
  NCollection_IndexedMap<TCollection_AsciiString> aMap;

  // Emplaced with new key should return reference to key
  const TCollection_AsciiString& aRef = aMap.Emplaced("First");
  EXPECT_TRUE(aRef.IsEqual("First"));
  EXPECT_EQ(1, aMap.Extent());
}

