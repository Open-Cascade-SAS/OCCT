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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
}

TEST_F(NCollection_DataMapTest, BindingAndAccess)
{
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;

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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;

  aMap.Bind(1, "One");

  // Test ChangeFind for modification
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  aMap.ChangeFind(1) = "Modified One";
  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_DataMapTest, Rebind)
{
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;

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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;

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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;

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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap1;
  aMap1.Bind(1, "One");
  aMap1.Bind(2, "Two");

  // Test assignment operator
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap2;
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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Finding non-existent key should throw exception
  EXPECT_THROW(aMap.Find(2), Standard_NoSuchObject);

  // ChangeFind for non-existent key should throw exception
  EXPECT_THROW(aMap.ChangeFind(2), Standard_NoSuchObject);
}

TEST_F(NCollection_DataMapTest, IteratorAccess)
{
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  // Test iteration using OCCT iterator
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString>::Iterator it(aMap);

  // Create sets to check all keys and values are visited
  std::set<Standard_Integer> foundKeys;
  std::set<std::string>      foundValues;

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
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString> aMap;
  aMap.Bind(1, "One");

  // Test Value change through iterator
  NCollection_DataMap<Standard_Integer, TCollection_AsciiString>::Iterator it(aMap);
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
  NCollection_DataMap<Standard_Integer, Standard_Integer> aMap;

  // Bind many elements
  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Bind(i, i * 2);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  // Count elements using iterator
  int                                                               count = 0;
  NCollection_DataMap<Standard_Integer, Standard_Integer>::Iterator it(aMap);
  for (; it.More(); it.Next())
  {
    EXPECT_EQ(it.Key() * 2, it.Value());
    count++;
  }

  EXPECT_EQ(NUM_ELEMENTS, count);
}

TEST_F(NCollection_DataMapTest, STLAlgorithmCompatibility_MinMax)
{
  NCollection_DataMap<Standard_Integer, Standard_Integer> aMap;

  // Add some sequential values to make results predictable
  for (Standard_Integer anIdx = 10; anIdx <= 50; anIdx += 5)
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
  NCollection_DataMap<Standard_Integer, Standard_Integer> aMap;

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
