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
