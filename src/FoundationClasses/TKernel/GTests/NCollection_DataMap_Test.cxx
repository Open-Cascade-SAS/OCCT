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

#include <NCollection_DataMap.hxx>
#include <Standard_Integer.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <algorithm>
#include <vector>
#include <set>

// Test fixture for NCollection_DataMap tests
class NCollection_DataMapTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

// Tests with Integer keys and String values
TEST_F(NCollection_DataMapTest, IntegerKeys)
{
  // Default constructor should create an empty map
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
}

TEST_F(NCollection_DataMapTest, BindingAndAccess)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // Test Bind method
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  EXPECT_FALSE(aMap.IsEmpty());
  EXPECT_EQ(3, aMap.Size());

  // Test Find method
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  EXPECT_STREQ("Two", aMap.Find(2).ToCString());
  EXPECT_STREQ("Three", aMap.Find(3).ToCString());

  // Test IsBound
  EXPECT_TRUE(aMap.IsBound(1));
  EXPECT_TRUE(aMap.IsBound(2));
  EXPECT_TRUE(aMap.IsBound(3));
  EXPECT_FALSE(aMap.IsBound(4));
}

TEST_F(NCollection_DataMapTest, ChangeFind)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  aMap.Bind(1, "One");

  // Test ChangeFind for modification
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  aMap.ChangeFind(1) = "Modified One";
  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, Rebind)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // First binding
  aMap.Bind(1, "One");
  EXPECT_STREQ("One", aMap.Find(1).ToCString());

  // Re-bind the same key with a different value
  aMap.Bind(1, "New One");
  EXPECT_STREQ("New One", aMap.Find(1).ToCString());

  // Size should still be 1
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_DataMapTest, UnBind)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  // Test UnBind
  EXPECT_TRUE(aMap.UnBind(2));
  EXPECT_EQ(2, aMap.Size());

  // Check key 2 is no longer bound
  EXPECT_FALSE(aMap.IsBound(2));

  // Try to unbind a non-existent key
  EXPECT_FALSE(aMap.UnBind(4));
  EXPECT_EQ(2, aMap.Size());
}

TEST_F(NCollection_DataMapTest, Clear)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  // Test Clear
  aMap.Clear();
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_FALSE(aMap.IsBound(1));
  EXPECT_FALSE(aMap.IsBound(2));
}

TEST_F(NCollection_DataMapTest, Assignment)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap1;
  aMap1.Bind(1, "One");
  aMap1.Bind(2, "Two");

  // Test assignment operator
  NCollection_DataMap<int, TCollection_AsciiString> aMap2;
  aMap2 = aMap1;

  // Check both maps have the same content
  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_STREQ(aMap1.Find(1).ToCString(), aMap2.Find(1).ToCString());
  EXPECT_STREQ(aMap1.Find(2).ToCString(), aMap2.Find(2).ToCString());

  // Modify original to ensure deep copy
  aMap1.ChangeFind(1) = "Modified One";
  EXPECT_STREQ("Modified One", aMap1.Find(1).ToCString());
  EXPECT_STREQ("One", aMap2.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, Find_NonExisting)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Finding non-existent key should throw exception
  EXPECT_THROW(aMap.Find(2), Standard_NoSuchObject);

  // ChangeFind for non-existent key should throw exception
  EXPECT_THROW(aMap.ChangeFind(2), Standard_NoSuchObject);
}

TEST_F(NCollection_DataMapTest, IteratorAccess)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  // Test iteration using OCCT iterator
  NCollection_DataMap<int, TCollection_AsciiString>::Iterator it(aMap);

  // Create sets to check all keys and values are visited
  std::set<int>         foundKeys;
  std::set<std::string> foundValues;

  for (; it.More(); it.Next())
  {
    foundKeys.insert(it.Key());
    foundValues.insert(it.Value().ToCString());
  }

  // Check all keys were visited
  EXPECT_EQ(3, foundKeys.size());
  EXPECT_TRUE(foundKeys.find(1) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(2) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(3) != foundKeys.end());

  // Check all values were visited
  EXPECT_EQ(3, foundValues.size());
  EXPECT_TRUE(foundValues.find("One") != foundValues.end());
  EXPECT_TRUE(foundValues.find("Two") != foundValues.end());
  EXPECT_TRUE(foundValues.find("Three") != foundValues.end());
}

TEST_F(NCollection_DataMapTest, ChangeValue)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Test Value change through iterator
  NCollection_DataMap<int, TCollection_AsciiString>::Iterator it(aMap);
  EXPECT_STREQ("One", it.Value().ToCString());

  it.ChangeValue() = "Modified via Iterator";
  EXPECT_STREQ("Modified via Iterator", it.Value().ToCString());

  // Check the change is reflected in the map itself
  EXPECT_STREQ("Modified via Iterator", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, ExhaustiveIterator)
{
  const int NUM_ELEMENTS = 1000;

  // Create a map with many elements to test iterator efficiency
  NCollection_DataMap<int, int> aMap;

  // Bind many elements
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Bind(i, i * 2);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  // Count elements using iterator
  int                                     count = 0;
  NCollection_DataMap<int, int>::Iterator it(aMap);
  for (; it.More(); it.Next())
  {
    EXPECT_EQ(it.Key() * 2, it.Value());
    count++;
  }

  EXPECT_EQ(NUM_ELEMENTS, count);
}

TEST_F(NCollection_DataMapTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_DataMap<int, int> aMap;

  // Add some sequential values to make results predictable
  for (int anIdx = 10; anIdx <= 50; anIdx += 5)
  {
    aMap.Bind(anIdx, anIdx * 2);
  }

  EXPECT_FALSE(aMap.IsEmpty());

  // Test that STL algorithms work with OCCT iterators
  auto aMinElement = std::min_element(aMap.cbegin(), aMap.cend());
  auto aMaxElement = std::max_element(aMap.cbegin(), aMap.cend());

  EXPECT_TRUE(aMinElement != aMap.cend());
  EXPECT_TRUE(aMaxElement != aMap.cend());
  EXPECT_LE(*aMinElement, *aMaxElement);
}

TEST_F(NCollection_DataMapTest, STLAlgorithmCompatibility_Find)
{
  NCollection_DataMap<int, int> aMap;

  // Add known values
  aMap.Bind(100, 200);
  aMap.Bind(200, 400);
  aMap.Bind(300, 600);

  // Test std::find compatibility
  auto aFound = std::find(aMap.cbegin(), aMap.cend(), 200);
  EXPECT_TRUE(aFound != aMap.cend());
  EXPECT_EQ(*aFound, 200);

  // Test finding non-existent value
  auto aNotFound = std::find(aMap.cbegin(), aMap.cend(), 999);
  EXPECT_TRUE(aNotFound == aMap.cend());
}

TEST_F(NCollection_DataMapTest, TryBind_NewKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // TryBind on new key should succeed
  bool aResult = aMap.TryBind(1, "One");
  EXPECT_TRUE(aResult);
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());

  // TryBind another new key
  aResult = aMap.TryBind(2, "Two");
  EXPECT_TRUE(aResult);
  EXPECT_EQ(2, aMap.Size());
  EXPECT_STREQ("Two", aMap.Find(2).ToCString());
}

TEST_F(NCollection_DataMapTest, TryBind_ExistingKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // First bind a key
  aMap.Bind(1, "One");
  EXPECT_STREQ("One", aMap.Find(1).ToCString());

  // TryBind on existing key should fail and NOT replace the value
  bool aResult = aMap.TryBind(1, "New One");
  EXPECT_FALSE(aResult);
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString()); // Value should be unchanged
}

TEST_F(NCollection_DataMapTest, TryBind_MoveSemantics)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // Test with rvalue key
  int  aKey    = 1;
  bool aResult = aMap.TryBind(std::move(aKey), "One");
  EXPECT_TRUE(aResult);
  EXPECT_STREQ("One", aMap.Find(1).ToCString());

  // Test with rvalue value
  TCollection_AsciiString aValue("Two");
  aResult = aMap.TryBind(2, std::move(aValue));
  EXPECT_TRUE(aResult);
  EXPECT_STREQ("Two", aMap.Find(2).ToCString());

  // Test with both rvalues
  aResult = aMap.TryBind(3, TCollection_AsciiString("Three"));
  EXPECT_TRUE(aResult);
  EXPECT_STREQ("Three", aMap.Find(3).ToCString());
}

TEST_F(NCollection_DataMapTest, TryBound_NewKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // TryBound on new key should add and return reference to new item
  TCollection_AsciiString& aRef = aMap.TryBound(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  EXPECT_EQ(1, aMap.Size());

  // Modify through reference
  aRef = "Modified One";
  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, TryBound_ExistingKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // First bind a key
  aMap.Bind(1, "One");

  // TryBound on existing key should return reference to existing item (not replace)
  TCollection_AsciiString& aRef = aMap.TryBound(1, "New One");
  EXPECT_STREQ("One", aRef.ToCString());         // Should be the original value
  EXPECT_STREQ("One", aMap.Find(1).ToCString()); // Map should have original value
  EXPECT_EQ(1, aMap.Size());

  // Modify through reference
  aRef = "Modified One";
  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, TryBound_MoveSemantics)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // Test with rvalue key
  TCollection_AsciiString& aRef1 = aMap.TryBound(1, "One");
  EXPECT_STREQ("One", aRef1.ToCString());

  // Test with rvalue value on new key
  TCollection_AsciiString& aRef2 = aMap.TryBound(2, TCollection_AsciiString("Two"));
  EXPECT_STREQ("Two", aRef2.ToCString());

  // Test with existing key - should not replace
  TCollection_AsciiString& aRef3 = aMap.TryBound(1, TCollection_AsciiString("New One"));
  EXPECT_STREQ("One", aRef3.ToCString()); // Should return original
}

// Tests for Emplace methods
TEST_F(NCollection_DataMapTest, Emplace_NewKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // Emplace with new key should return true
  EXPECT_TRUE(aMap.Emplace(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());

  // Emplace another new key
  EXPECT_TRUE(aMap.Emplace(2, "Two"));
  EXPECT_EQ(2, aMap.Size());
}

TEST_F(NCollection_DataMapTest, Emplace_ExistingKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Emplace on existing key should destroy and reconstruct (return false)
  EXPECT_FALSE(aMap.Emplace(1, "New One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("New One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, Emplaced_NewKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // Emplaced with new key should return reference to new value
  TCollection_AsciiString& aRef = aMap.Emplaced(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_DataMapTest, Emplaced_ExistingKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Emplaced on existing key should destroy+reconstruct and return reference
  TCollection_AsciiString& aRef = aMap.Emplaced(1, "New One");
  EXPECT_STREQ("New One", aRef.ToCString());
}

TEST_F(NCollection_DataMapTest, TryEmplace_NewKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // TryEmplace with new key should return true
  EXPECT_TRUE(aMap.TryEmplace(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, TryEmplace_ExistingKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // TryEmplace on existing key should NOT modify (return false)
  EXPECT_FALSE(aMap.TryEmplace(1, "New One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString()); // Still original value
}

TEST_F(NCollection_DataMapTest, TryEmplaced_NewKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;

  // TryEmplaced with new key should return reference to new value
  TCollection_AsciiString& aRef = aMap.TryEmplaced(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_DataMapTest, TryEmplaced_ExistingKey)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // TryEmplaced on existing key should return reference to existing (no modification)
  TCollection_AsciiString& aRef = aMap.TryEmplaced(1, "New One");
  EXPECT_STREQ("One", aRef.ToCString()); // Original value
}

// Tests for hasher constructor
TEST_F(NCollection_DataMapTest, HasherConstructorCopy)
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

    bool operator()(int theKey1, int theKey2) const { return theKey1 == theKey2; }
  };

  StatefulHasher                                                    aHasher(42);
  NCollection_DataMap<int, TCollection_AsciiString, StatefulHasher> aMap(aHasher, 10);

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  EXPECT_EQ(2, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  EXPECT_STREQ("Two", aMap.Find(2).ToCString());

  // Verify hasher was copied
  const StatefulHasher& aMapHasher = aMap.GetHasher();
  EXPECT_EQ(42, aMapHasher.mySalt);
}

TEST_F(NCollection_DataMapTest, HasherConstructorMove)
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

  NCollection_DataMap<int, TCollection_AsciiString, StatefulHasher> aMap(StatefulHasher(99), 10);

  aMap.Bind(10, "Ten");

  EXPECT_EQ(1, aMap.Size());
  EXPECT_EQ(99, aMap.GetHasher().mySalt);
}

TEST_F(NCollection_DataMapTest, CopyConstructorPreservesHasher)
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

  NCollection_DataMap<int, TCollection_AsciiString, StatefulHasher> aMap1(StatefulHasher(123), 10);
  aMap1.Bind(1, "One");

  // Copy construct
  NCollection_DataMap<int, TCollection_AsciiString, StatefulHasher> aMap2(aMap1);

  EXPECT_EQ(123, aMap2.GetHasher().mySalt);
  EXPECT_EQ(1, aMap2.Size());
  EXPECT_STREQ("One", aMap2.Find(1).ToCString());
}

// Tests for Items() key-value pair iteration
TEST_F(NCollection_DataMapTest, ItemsIteration)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  // Test Items() iteration
  std::set<int>         aFoundKeys;
  std::set<std::string> aFoundValues;
  for (auto aKeyValue : aMap.Items())
  {
    aFoundKeys.insert(aKeyValue.Key);
    aFoundValues.insert(aKeyValue.Value.ToCString());
  }

  EXPECT_EQ(3u, aFoundKeys.size());
  EXPECT_TRUE(aFoundKeys.count(1) > 0);
  EXPECT_TRUE(aFoundKeys.count(2) > 0);
  EXPECT_TRUE(aFoundKeys.count(3) > 0);

  EXPECT_EQ(3u, aFoundValues.size());
  EXPECT_TRUE(aFoundValues.count("One") > 0);
  EXPECT_TRUE(aFoundValues.count("Two") > 0);
  EXPECT_TRUE(aFoundValues.count("Three") > 0);
}

TEST_F(NCollection_DataMapTest, ItemsStructuredBindings)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  // Test structured bindings with Items()
  int aSum = 0;
  for (auto [aKey, aValue] : aMap.Items())
  {
    aSum += aKey;
    (void)aValue; // Suppress unused warning
  }

  EXPECT_EQ(30, aSum);
}

TEST_F(NCollection_DataMapTest, ItemsModifyValue)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "Original");

  // Test modifying values through Items()
  for (auto [aKey, aValue] : aMap.Items())
  {
    (void)aKey;
    aValue = "Modified";
  }

  EXPECT_STREQ("Modified", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, ConstItemsIteration)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  const NCollection_DataMap<int, TCollection_AsciiString>& aConstMap = aMap;

  // Test const Items() iteration
  int aCount = 0;
  for (const auto& [aKey, aValue] : aConstMap.Items())
  {
    (void)aKey;
    (void)aValue;
    ++aCount;
  }

  EXPECT_EQ(2, aCount);
}

// Test iterator equality for Items() view
TEST_F(NCollection_DataMapTest, ItemsIteratorEquality)
{
  NCollection_DataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  auto aView = aMap.Items();

  // begin() should equal begin()
  EXPECT_EQ(aView.begin(), aView.begin());

  // end() should equal end()
  EXPECT_EQ(aView.end(), aView.end());

  // begin() should not equal end() when map is not empty
  EXPECT_NE(aView.begin(), aView.end());

  // Two iterators at different positions must NOT be equal
  auto anIt1 = aView.begin();
  auto anIt2 = aView.begin();
  ++anIt2;
  EXPECT_NE(anIt1, anIt2);

  // Iterators at same position should be equal
  auto anIt3 = aView.begin();
  ++anIt3;
  EXPECT_EQ(anIt2, anIt3);
}
