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

#include <NCollection_IndexedDataMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>

// Basic test types for the IndexedDataMap
typedef Standard_Integer KeyType;
typedef Standard_Real    ItemType;

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

namespace std
{
template <>
struct hash<TestKey>
{
  size_t operator()(const TestKey& key) const
  {
    // Combine the hash
    size_t aCombintation[2] = {std::hash<int>()(key.GetId()),
                               std::hash<std::string>()(key.GetName())};
    return opencascade::hashBytes(aCombintation, sizeof(aCombintation));
  }
};
} // namespace std

// Custom class for testing complex items
class TestItem
{
public:
  TestItem(double value = 0.0, const char* description = "")
      : myValue(value),
        myDescription(description)
  {
  }

  bool operator==(const TestItem& other) const
  {
    return fabs(myValue - other.myValue) < 1e-10 && myDescription == other.myDescription;
  }

  double GetValue() const { return myValue; }

  const std::string& GetDescription() const { return myDescription; }

  void SetValue(double value) { myValue = value; }

private:
  double      myValue;
  std::string myDescription;
};

TEST(NCollection_IndexedDataMapTest, DefaultConstructor)
{
  // Test default constructor
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
  EXPECT_EQ(aMap.Size(), 0);
}

TEST(NCollection_IndexedDataMapTest, BasicAddFind)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Test adding elements
  Standard_Integer index1 = aMap.Add(10, 1.0);
  Standard_Integer index2 = aMap.Add(20, 2.0);
  Standard_Integer index3 = aMap.Add(30, 3.0);

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

  // Test finding items by index
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(1), 1.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(2), 2.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(3), 3.0);

  // Test operator()
  EXPECT_DOUBLE_EQ(aMap(1), 1.0);
  EXPECT_DOUBLE_EQ(aMap(2), 2.0);
  EXPECT_DOUBLE_EQ(aMap(3), 3.0);

  // Test finding items by key
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 2.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(30), 3.0);

  // Test Contains
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_TRUE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));
  EXPECT_FALSE(aMap.Contains(40));
}

TEST(NCollection_IndexedDataMapTest, DuplicateKey)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add initial elements
  Standard_Integer index1 = aMap.Add(10, 1.0);
  Standard_Integer index2 = aMap.Add(20, 2.0);

  EXPECT_EQ(index1, 1);
  EXPECT_EQ(index2, 2);

  // Try to add a duplicate - should return the existing index and not change the item
  Standard_Integer indexDup = aMap.Add(10, 3.0);
  EXPECT_EQ(indexDup, 1);
  EXPECT_EQ(aMap.Extent(), 2);                 // Size should not change
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.0); // Original value should be preserved
}

TEST(NCollection_IndexedDataMapTest, ChangeValues)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);

  // Change values through ChangeFromIndex
  aMap.ChangeFromIndex(1) = 1.5;
  aMap.ChangeFromIndex(2) = 2.5;

  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(1), 1.5);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(2), 2.5);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.5);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 2.5);

  // Change values through ChangeFromKey
  aMap.ChangeFromKey(10) = 1.75;
  aMap.ChangeFromKey(20) = 2.75;

  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(1), 1.75);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(2), 2.75);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.75);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 2.75);

  // Change values through operator()
  aMap(1) = 1.9;
  aMap(2) = 2.9;

  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(1), 1.9);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(2), 2.9);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.9);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 2.9);
}

TEST(NCollection_IndexedDataMapTest, SeekTests)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);

  // Test Seek
  const ItemType* item1 = aMap.Seek(10);
  const ItemType* item2 = aMap.Seek(20);
  const ItemType* item3 = aMap.Seek(30); // Not present

  EXPECT_TRUE(item1 != nullptr);
  EXPECT_TRUE(item2 != nullptr);
  EXPECT_TRUE(item3 == nullptr);

  EXPECT_DOUBLE_EQ(*item1, 1.0);
  EXPECT_DOUBLE_EQ(*item2, 2.0);

  // Test ChangeSeek
  ItemType* changeItem1 = aMap.ChangeSeek(10);
  ItemType* changeItem2 = aMap.ChangeSeek(20);
  ItemType* changeItem3 = aMap.ChangeSeek(30); // Not present

  EXPECT_TRUE(changeItem1 != nullptr);
  EXPECT_TRUE(changeItem2 != nullptr);
  EXPECT_TRUE(changeItem3 == nullptr);

  *changeItem1 = 1.5;
  *changeItem2 = 2.5;

  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.5);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 2.5);

  // Test FindFromKey with copy
  ItemType value;
  bool     found = aMap.FindFromKey(10, value);
  EXPECT_TRUE(found);
  EXPECT_DOUBLE_EQ(value, 1.5);

  found = aMap.FindFromKey(30, value);
  EXPECT_FALSE(found);
}

TEST(NCollection_IndexedDataMapTest, RemoveLast)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

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

TEST(NCollection_IndexedDataMapTest, RemoveFromIndex)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);
  aMap.Add(40, 4.0);

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
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(2), 4.0);

  EXPECT_EQ(aMap.FindKey(1), 10); // First element unchanged
  EXPECT_EQ(aMap.FindKey(3), 30); // Third element unchanged
}

TEST(NCollection_IndexedDataMapTest, RemoveKey)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

  // Remove by key
  aMap.RemoveKey(20);

  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_FALSE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(30));

  // Try to remove non-existent key
  aMap.RemoveKey(50); // Should have no effect
  EXPECT_EQ(aMap.Extent(), 2);
}

TEST(NCollection_IndexedDataMapTest, Substitute)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

  // Substitute key and value at index 2
  aMap.Substitute(2, 25, 2.5);

  EXPECT_EQ(aMap.Extent(), 3);
  EXPECT_TRUE(aMap.Contains(10));
  EXPECT_FALSE(aMap.Contains(20));
  EXPECT_TRUE(aMap.Contains(25));
  EXPECT_TRUE(aMap.Contains(30));

  // Check indices after substitution
  EXPECT_EQ(aMap.FindIndex(10), 1);
  EXPECT_EQ(aMap.FindIndex(25), 2);
  EXPECT_EQ(aMap.FindIndex(30), 3);

  // Check values after substitution
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(25), 2.5);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(30), 3.0);
}

TEST(NCollection_IndexedDataMapTest, Swap)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

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

  // And values follow their keys
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(1), 3.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(2), 2.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromIndex(3), 1.0);

  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 2.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(30), 3.0);
}

TEST(NCollection_IndexedDataMapTest, Clear)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

  EXPECT_EQ(aMap.Extent(), 3);

  // Clear the map
  aMap.Clear();

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);

  // Ensure we can add elements again
  Standard_Integer index1 = aMap.Add(40, 4.0);
  EXPECT_EQ(index1, 1);
  EXPECT_EQ(aMap.Extent(), 1);
}

TEST(NCollection_IndexedDataMapTest, CopyConstructor)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap1;

  // Add elements to first map
  aMap1.Add(10, 1.0);
  aMap1.Add(20, 2.0);
  aMap1.Add(30, 3.0);

  // Create a copy
  NCollection_IndexedDataMap<KeyType, ItemType> aMap2(aMap1);

  // Check that copy has the same elements
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
  EXPECT_TRUE(aMap2.Contains(30));

  // Check indices
  EXPECT_EQ(aMap2.FindIndex(10), 1);
  EXPECT_EQ(aMap2.FindIndex(20), 2);
  EXPECT_EQ(aMap2.FindIndex(30), 3);

  // Check values
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(20), 2.0);
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(30), 3.0);

  // Modify original to ensure deep copy
  aMap1.Add(40, 4.0);
  EXPECT_EQ(aMap1.Extent(), 4);
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_FALSE(aMap2.Contains(40));
}

TEST(NCollection_IndexedDataMapTest, AssignmentOperator)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap1;
  NCollection_IndexedDataMap<KeyType, ItemType> aMap2;

  // Add elements to first map
  aMap1.Add(10, 1.0);
  aMap1.Add(20, 2.0);

  // Add different elements to second map
  aMap2.Add(30, 3.0);
  aMap2.Add(40, 4.0);
  aMap2.Add(50, 5.0);

  // Assign first to second
  aMap2 = aMap1;

  // Check that second map now matches first
  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
  EXPECT_FALSE(aMap2.Contains(30));
  EXPECT_FALSE(aMap2.Contains(40));
  EXPECT_FALSE(aMap2.Contains(50));

  // Check values
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(20), 2.0);
}

TEST(NCollection_IndexedDataMapTest, Iterator)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

  // Use iterator to check all elements
  Standard_Boolean found10 = Standard_False;
  Standard_Boolean found20 = Standard_False;
  Standard_Boolean found30 = Standard_False;
  Standard_Size    count   = 0;

  for (NCollection_IndexedDataMap<KeyType, ItemType>::Iterator it(aMap); it.More();
       it.Next(), ++count)
  {
    const KeyType&  key   = it.Key();
    const ItemType& value = it.Value();

    if (key == 10 && value == 1.0)
      found10 = Standard_True;
    else if (key == 20 && value == 2.0)
      found20 = Standard_True;
    else if (key == 30 && value == 3.0)
      found30 = Standard_True;
  }

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(found10);
  EXPECT_TRUE(found20);
  EXPECT_TRUE(found30);

  // Test modification through iterator
  for (NCollection_IndexedDataMap<KeyType, ItemType>::Iterator it(aMap); it.More(); it.Next())
  {
    it.ChangeValue() *= 2;
  }

  EXPECT_DOUBLE_EQ(aMap.FindFromKey(10), 2.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(20), 4.0);
  EXPECT_DOUBLE_EQ(aMap.FindFromKey(30), 6.0);
}

TEST(NCollection_IndexedDataMapTest, StlIterator)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap;

  // Add elements
  aMap.Add(10, 1.0);
  aMap.Add(20, 2.0);
  aMap.Add(30, 3.0);

  // Use STL-style iterator
  Standard_Boolean found1 = Standard_False;
  Standard_Boolean found2 = Standard_False;
  Standard_Boolean found3 = Standard_False;
  Standard_Size    count  = 0;

  for (auto it = aMap.begin(); it != aMap.end(); ++it, ++count)
  {
    if (*it == 1.0)
      found1 = Standard_True;
    else if (*it == 2.0)
      found2 = Standard_True;
    else if (*it == 3.0)
      found3 = Standard_True;
  }

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(found1);
  EXPECT_TRUE(found2);
  EXPECT_TRUE(found3);

  // Test const iterator
  count  = 0;
  found1 = Standard_False;
  found2 = Standard_False;
  found3 = Standard_False;

  for (auto it = aMap.cbegin(); it != aMap.cend(); ++it, ++count)
  {
    if (*it == 1.0)
      found1 = Standard_True;
    else if (*it == 2.0)
      found2 = Standard_True;
    else if (*it == 3.0)
      found3 = Standard_True;
  }

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(found1);
  EXPECT_TRUE(found2);
  EXPECT_TRUE(found3);
}

TEST(NCollection_IndexedDataMapTest, StringKeys)
{
  // Test with string keys
  NCollection_IndexedDataMap<TCollection_AsciiString, ItemType> aStringMap;

  // Add string keys
  Standard_Integer index1 = aStringMap.Add(TCollection_AsciiString("First"), 1.0);
  Standard_Integer index2 = aStringMap.Add(TCollection_AsciiString("Second"), 2.0);
  Standard_Integer index3 = aStringMap.Add(TCollection_AsciiString("Third"), 3.0);

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

  // Find values
  EXPECT_DOUBLE_EQ(aStringMap.FindFromKey(TCollection_AsciiString("First")), 1.0);
  EXPECT_DOUBLE_EQ(aStringMap.FindFromKey(TCollection_AsciiString("Second")), 2.0);
  EXPECT_DOUBLE_EQ(aStringMap.FindFromKey(TCollection_AsciiString("Third")), 3.0);
}

TEST(NCollection_IndexedDataMapTest, ComplexKeyAndValue)
{
  // Create map with custom key and hasher
  NCollection_IndexedDataMap<TestKey, TestItem> aComplexMap;

  // Add complex keys and values
  TestKey key1(1, "One");
  TestKey key2(2, "Two");
  TestKey key3(3, "Three");

  TestItem item1(1.1, "Item One");
  TestItem item2(2.2, "Item Two");
  TestItem item3(3.3, "Item Three");

  Standard_Integer index1 = aComplexMap.Add(key1, item1);
  Standard_Integer index2 = aComplexMap.Add(key2, item2);
  Standard_Integer index3 = aComplexMap.Add(key3, item3);

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

  // Find values
  EXPECT_DOUBLE_EQ(aComplexMap.FindFromKey(key1).GetValue(), 1.1);
  EXPECT_DOUBLE_EQ(aComplexMap.FindFromKey(key2).GetValue(), 2.2);
  EXPECT_DOUBLE_EQ(aComplexMap.FindFromKey(key3).GetValue(), 3.3);

  // Test contains
  EXPECT_TRUE(aComplexMap.Contains(TestKey(1, "One")));
  EXPECT_FALSE(aComplexMap.Contains(TestKey(4, "Four")));

  // Test value modification
  aComplexMap.ChangeFromKey(key1).SetValue(1.5);
  EXPECT_DOUBLE_EQ(aComplexMap.FindFromKey(key1).GetValue(), 1.5);
}

TEST(NCollection_IndexedDataMapTest, Exchange)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap1;
  NCollection_IndexedDataMap<KeyType, ItemType> aMap2;

  // Add elements to first map
  aMap1.Add(10, 1.0);
  aMap1.Add(20, 2.0);

  // Add different elements to second map
  aMap2.Add(30, 3.0);
  aMap2.Add(40, 4.0);
  aMap2.Add(50, 5.0);

  // Exchange maps
  aMap1.Exchange(aMap2);

  // Check that maps are exchanged
  EXPECT_EQ(aMap1.Extent(), 3);
  EXPECT_TRUE(aMap1.Contains(30));
  EXPECT_TRUE(aMap1.Contains(40));
  EXPECT_TRUE(aMap1.Contains(50));
  EXPECT_DOUBLE_EQ(aMap1.FindFromKey(30), 3.0);
  EXPECT_DOUBLE_EQ(aMap1.FindFromKey(40), 4.0);
  EXPECT_DOUBLE_EQ(aMap1.FindFromKey(50), 5.0);

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.Contains(10));
  EXPECT_TRUE(aMap2.Contains(20));
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap2.FindFromKey(20), 2.0);
}

TEST(NCollection_IndexedDataMapTest, ReSize)
{
  NCollection_IndexedDataMap<KeyType, ItemType> aMap(3); // Start with small bucket count

  // Add many elements to trigger resize
  for (Standard_Integer i = 1; i <= 100; ++i)
  {
    aMap.Add(i, static_cast<Standard_Real>(i) / 10.0);
  }

  // Verify all elements are present
  EXPECT_EQ(aMap.Extent(), 100);
  for (Standard_Integer i = 1; i <= 100; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
    EXPECT_EQ(aMap.FindIndex(i), i);
    EXPECT_DOUBLE_EQ(aMap.FindFromKey(i), static_cast<Standard_Real>(i) / 10.0);
  }

  // Explicitly resize
  aMap.ReSize(200);

  // Check that elements are still accessible
  EXPECT_EQ(aMap.Extent(), 100);
  for (Standard_Integer i = 1; i <= 100; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i));
    EXPECT_EQ(aMap.FindIndex(i), i);
    EXPECT_DOUBLE_EQ(aMap.FindFromKey(i), static_cast<Standard_Real>(i) / 10.0);
  }
}

TEST(NCollection_IndexedDataMapTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_IndexedDataMap<Standard_Integer, Standard_Integer> aMap;

  // Add some sequential values to make results predictable
  for (Standard_Integer anIdx = 10; anIdx <= 50; anIdx += 5)
  {
    aMap.Add(anIdx, anIdx * 2);
  }

  EXPECT_FALSE(aMap.IsEmpty());

  // Test that STL algorithms work with OCCT iterators
  auto aMinElement = std::min_element(aMap.cbegin(), aMap.cend());
  auto aMaxElement = std::max_element(aMap.cbegin(), aMap.cend());

  EXPECT_TRUE(aMinElement != aMap.cend());
  EXPECT_TRUE(aMaxElement != aMap.cend());
  EXPECT_LE(*aMinElement, *aMaxElement);
}

TEST(NCollection_IndexedDataMapTest, STLAlgorithmCompatibility_Find)
{
  NCollection_IndexedDataMap<Standard_Integer, Standard_Integer> aMap;

  // Add known values
  aMap.Add(100, 200);
  aMap.Add(200, 400);
  aMap.Add(300, 600);

  // Test std::find compatibility
  auto aFound = std::find(aMap.cbegin(), aMap.cend(), 200);
  EXPECT_TRUE(aFound != aMap.cend());
  EXPECT_EQ(*aFound, 200);

  // Test finding non-existent value
  auto aNotFound = std::find(aMap.cbegin(), aMap.cend(), 999);
  EXPECT_TRUE(aNotFound == aMap.cend());
}
