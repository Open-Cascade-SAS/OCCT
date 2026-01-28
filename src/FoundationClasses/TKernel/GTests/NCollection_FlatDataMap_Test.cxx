// Copyright (c) 2024 OPEN CASCADE SAS
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

#include <NCollection_FlatDataMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <set>

//==================================================================================================
// NCollection_FlatDataMap Tests
//==================================================================================================

class NCollection_FlatDataMapTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NCollection_FlatDataMapTest, DefaultConstructor)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
}

TEST_F(NCollection_FlatDataMapTest, BindingAndAccess)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // Test Bind method
  EXPECT_TRUE(aMap.Bind(1, "One"));   // New key
  EXPECT_TRUE(aMap.Bind(2, "Two"));   // New key
  EXPECT_TRUE(aMap.Bind(3, "Three")); // New key

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

TEST_F(NCollection_FlatDataMapTest, SeekMethod)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Seek existing key
  const TCollection_AsciiString* aPtr = aMap.Seek(1);
  ASSERT_NE(nullptr, aPtr);
  EXPECT_STREQ("One", aPtr->ToCString());

  // Seek non-existing key
  EXPECT_EQ(nullptr, aMap.Seek(2));
}

TEST_F(NCollection_FlatDataMapTest, ChangeSeekMethod)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // ChangeSeek and modify
  TCollection_AsciiString* aPtr = aMap.ChangeSeek(1);
  ASSERT_NE(nullptr, aPtr);
  *aPtr = "Modified One";

  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, Rebind)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // First binding
  EXPECT_TRUE(aMap.Bind(1, "One")); // Returns true - new key
  EXPECT_STREQ("One", aMap.Find(1).ToCString());

  // Re-bind the same key with a different value
  EXPECT_FALSE(aMap.Bind(1, "New One")); // Returns false - existing key
  EXPECT_STREQ("New One", aMap.Find(1).ToCString());

  // Size should still be 1
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatDataMapTest, UnBind)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

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

TEST_F(NCollection_FlatDataMapTest, Clear)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  // Test Clear without releasing memory
  aMap.Clear(false);
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_FALSE(aMap.IsBound(1));
  EXPECT_FALSE(aMap.IsBound(2));

  // Map should still have capacity
  EXPECT_GT(aMap.Capacity(), 0u);
}

TEST_F(NCollection_FlatDataMapTest, ClearWithRelease)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  // Test Clear with releasing memory
  aMap.Clear(true);
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0u, aMap.Capacity());
}

TEST_F(NCollection_FlatDataMapTest, CopyConstructor)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap1;
  aMap1.Bind(1, "One");
  aMap1.Bind(2, "Two");

  // Test copy constructor
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap2(aMap1);

  // Check both maps have the same content
  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_STREQ(aMap1.Find(1).ToCString(), aMap2.Find(1).ToCString());
  EXPECT_STREQ(aMap1.Find(2).ToCString(), aMap2.Find(2).ToCString());

  // Modify original to ensure deep copy
  aMap1.Bind(1, "Modified One");
  EXPECT_STREQ("Modified One", aMap1.Find(1).ToCString());
  EXPECT_STREQ("One", aMap2.Find(1).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, MoveConstructor)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap1;
  aMap1.Bind(1, "One");
  aMap1.Bind(2, "Two");

  // Test move constructor
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap2(std::move(aMap1));

  EXPECT_EQ(2, aMap2.Size());
  EXPECT_STREQ("One", aMap2.Find(1).ToCString());
  EXPECT_STREQ("Two", aMap2.Find(2).ToCString());

  // Original should be empty
  EXPECT_TRUE(aMap1.IsEmpty());
}

TEST_F(NCollection_FlatDataMapTest, Assignment)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap1;
  aMap1.Bind(1, "One");
  aMap1.Bind(2, "Two");

  // Test assignment operator
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap2;
  aMap2 = aMap1;

  // Check both maps have the same content
  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_STREQ(aMap1.Find(1).ToCString(), aMap2.Find(1).ToCString());
  EXPECT_STREQ(aMap1.Find(2).ToCString(), aMap2.Find(2).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, MoveAssignment)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap1;
  aMap1.Bind(1, "One");
  aMap1.Bind(2, "Two");

  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap2;
  aMap2 = std::move(aMap1);

  EXPECT_EQ(2, aMap2.Size());
  EXPECT_TRUE(aMap1.IsEmpty());
}

TEST_F(NCollection_FlatDataMapTest, Find_NonExisting)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Finding non-existent key should throw exception
  EXPECT_THROW(aMap.Find(2), Standard_NoSuchObject);

  // ChangeFind for non-existent key should throw exception
  EXPECT_THROW(aMap.ChangeFind(2), Standard_NoSuchObject);
}

TEST_F(NCollection_FlatDataMapTest, IteratorAccess)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  // Test iteration
  NCollection_FlatDataMap<int, TCollection_AsciiString>::Iterator it(aMap);

  std::set<int>         foundKeys;
  std::set<std::string> foundValues;

  for (; it.More(); it.Next())
  {
    foundKeys.insert(it.Key());
    foundValues.insert(it.Value().ToCString());
  }

  // Check all keys were visited
  EXPECT_EQ(3u, foundKeys.size());
  EXPECT_TRUE(foundKeys.find(1) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(2) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(3) != foundKeys.end());

  // Check all values were visited
  EXPECT_EQ(3u, foundValues.size());
  EXPECT_TRUE(foundValues.find("One") != foundValues.end());
  EXPECT_TRUE(foundValues.find("Two") != foundValues.end());
  EXPECT_TRUE(foundValues.find("Three") != foundValues.end());
}

TEST_F(NCollection_FlatDataMapTest, LargeDataSet)
{
  const int NUM_ELEMENTS = 10000;

  NCollection_FlatDataMap<int, int> aMap;

  // Bind many elements
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Bind(i, i * 2);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  // Verify all elements
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    ASSERT_TRUE(aMap.IsBound(i)) << "Key " << i << " not found";
    EXPECT_EQ(i * 2, aMap.Find(i));
  }

  // Count elements using iterator
  int                                         count = 0;
  NCollection_FlatDataMap<int, int>::Iterator it(aMap);
  for (; it.More(); it.Next())
  {
    EXPECT_EQ(it.Key() * 2, it.Value());
    count++;
  }

  EXPECT_EQ(NUM_ELEMENTS, count);
}

TEST_F(NCollection_FlatDataMapTest, UnBindAndRebind)
{
  NCollection_FlatDataMap<int, int> aMap;

  // Bind, unbind, rebind pattern
  for (int i = 0; i < 100; ++i)
  {
    aMap.Bind(i, i);
  }

  // Unbind even keys
  for (int i = 0; i < 100; i += 2)
  {
    EXPECT_TRUE(aMap.UnBind(i));
  }

  EXPECT_EQ(50, aMap.Size());

  // Rebind even keys with different values
  for (int i = 0; i < 100; i += 2)
  {
    aMap.Bind(i, i * 10);
  }

  EXPECT_EQ(100, aMap.Size());

  // Verify
  for (int i = 0; i < 100; ++i)
  {
    if (i % 2 == 0)
    {
      EXPECT_EQ(i * 10, aMap.Find(i));
    }
    else
    {
      EXPECT_EQ(i, aMap.Find(i));
    }
  }
}

TEST_F(NCollection_FlatDataMapTest, IntegerKeyIntegerValue)
{
  NCollection_FlatDataMap<int, int> aMap;

  aMap.Bind(10, 100);
  aMap.Bind(20, 200);
  aMap.Bind(30, 300);

  EXPECT_EQ(100, aMap(10));
  EXPECT_EQ(200, aMap(20));
  EXPECT_EQ(300, aMap(30));
}

// Tests for TryBind method
TEST_F(NCollection_FlatDataMapTest, TryBindNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // TryBind on new key - should succeed
  EXPECT_TRUE(aMap.TryBind(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, TryBindExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // TryBind on existing key - should NOT modify
  EXPECT_FALSE(aMap.TryBind(1, "New One"));
  EXPECT_STREQ("One", aMap.Find(1).ToCString()); // Original value preserved
  EXPECT_EQ(1, aMap.Size());
}

// Tests for TryBound method
TEST_F(NCollection_FlatDataMapTest, TryBoundNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // TryBound on new key - should return reference to new value
  TCollection_AsciiString& aRef = aMap.TryBound(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatDataMapTest, TryBoundExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // TryBound on existing key - should return existing without modification
  TCollection_AsciiString& aRef = aMap.TryBound(1, "New One");
  EXPECT_STREQ("One", aRef.ToCString()); // Original value preserved
  EXPECT_EQ(1, aMap.Size());
}

// Tests for Bound method
TEST_F(NCollection_FlatDataMapTest, BoundNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // Bound on new key
  TCollection_AsciiString& aRef = aMap.Bound(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatDataMapTest, BoundExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Bound on existing key - should overwrite
  TCollection_AsciiString& aRef = aMap.Bound(1, "New One");
  EXPECT_STREQ("New One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

// Tests for Emplace method
TEST_F(NCollection_FlatDataMapTest, EmplaceNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // Emplace with new key
  EXPECT_TRUE(aMap.Emplace(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, EmplaceExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Emplace on existing key - destroys and reconstructs value
  EXPECT_FALSE(aMap.Emplace(1, "New One"));
  EXPECT_STREQ("New One", aMap.Find(1).ToCString());
  EXPECT_EQ(1, aMap.Size());
}

// Tests for Emplaced method
TEST_F(NCollection_FlatDataMapTest, EmplacedNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // Emplaced with new key
  TCollection_AsciiString& aRef = aMap.Emplaced(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatDataMapTest, EmplacedExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Emplaced on existing - destroys and reconstructs
  TCollection_AsciiString& aRef = aMap.Emplaced(1, "New One");
  EXPECT_STREQ("New One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

// Tests for TryEmplace method
TEST_F(NCollection_FlatDataMapTest, TryEmplaceNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // TryEmplace with new key
  EXPECT_TRUE(aMap.TryEmplace(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, TryEmplaceExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // TryEmplace on existing - should NOT modify
  EXPECT_FALSE(aMap.TryEmplace(1, "New One"));
  EXPECT_STREQ("One", aMap.Find(1).ToCString()); // Original preserved
  EXPECT_EQ(1, aMap.Size());
}

// Tests for TryEmplaced method
TEST_F(NCollection_FlatDataMapTest, TryEmplacedNewKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;

  // TryEmplaced with new key
  TCollection_AsciiString& aRef = aMap.TryEmplaced(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatDataMapTest, TryEmplacedExistingKey)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // TryEmplaced on existing - should return existing without modification
  TCollection_AsciiString& aRef = aMap.TryEmplaced(1, "New One");
  EXPECT_STREQ("One", aRef.ToCString()); // Original preserved
  EXPECT_EQ(1, aMap.Size());
}

// Tests for hasher constructor
TEST_F(NCollection_FlatDataMapTest, HasherConstructorCopy)
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

  StatefulHasher                                                        aHasher(42);
  NCollection_FlatDataMap<int, TCollection_AsciiString, StatefulHasher> aMap(aHasher, 10);

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  EXPECT_EQ(2, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  EXPECT_STREQ("Two", aMap.Find(2).ToCString());

  // Verify hasher was copied
  const StatefulHasher& aMapHasher = aMap.GetHasher();
  EXPECT_EQ(42, aMapHasher.mySalt);
}

TEST_F(NCollection_FlatDataMapTest, HasherConstructorMove)
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

  NCollection_FlatDataMap<int, TCollection_AsciiString, StatefulHasher> aMap(StatefulHasher(99),
                                                                             10);

  aMap.Bind(10, "Ten");

  EXPECT_EQ(1, aMap.Size());
  EXPECT_EQ(99, aMap.GetHasher().mySalt);
}

TEST_F(NCollection_FlatDataMapTest, CopyConstructorPreservesHasher)
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

  NCollection_FlatDataMap<int, TCollection_AsciiString, StatefulHasher> aMap1(StatefulHasher(123),
                                                                              10);
  aMap1.Bind(1, "One");

  // Copy construct
  NCollection_FlatDataMap<int, TCollection_AsciiString, StatefulHasher> aMap2(aMap1);

  EXPECT_EQ(123, aMap2.GetHasher().mySalt);
  EXPECT_EQ(1, aMap2.Size());
  EXPECT_STREQ("One", aMap2.Find(1).ToCString());
}

// Tests for Items() key-value pair iteration
TEST_F(NCollection_FlatDataMapTest, ItemsIteration)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
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

TEST_F(NCollection_FlatDataMapTest, ItemsStructuredBindings)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  // Test structured bindings with Items()
  int aSum = 0;
  for (auto [aKey, aValue] : aMap.Items())
  {
    aSum += aKey;
    (void)aValue;
  }

  EXPECT_EQ(30, aSum);
}

TEST_F(NCollection_FlatDataMapTest, ItemsModifyValue)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "Original");

  // Test modifying values through Items()
  for (auto [aKey, aValue] : aMap.Items())
  {
    (void)aKey;
    aValue = "Modified";
  }

  EXPECT_STREQ("Modified", aMap.Find(1).ToCString());
}

TEST_F(NCollection_FlatDataMapTest, ConstItemsIteration)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  const NCollection_FlatDataMap<int, TCollection_AsciiString>& aConstMap = aMap;

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
TEST_F(NCollection_FlatDataMapTest, ItemsIteratorEquality)
{
  NCollection_FlatDataMap<int, TCollection_AsciiString> aMap;
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
