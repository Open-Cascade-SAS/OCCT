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

#include <NCollection_OrderedDataMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <vector>

// Test fixture for NCollection_OrderedDataMap tests
class NCollection_OrderedDataMapTest : public testing::Test
{
protected:
  using IntStringMap = NCollection_OrderedDataMap<int, TCollection_AsciiString>;
  using IntIntMap    = NCollection_OrderedDataMap<int, int>;

  //! Collects keys from the ordered data map in iteration order.
  static std::vector<int> collectKeys(const IntStringMap& theMap)
  {
    std::vector<int> aKeys;
    for (IntStringMap::Iterator anIter(theMap); anIter.More(); anIter.Next())
    {
      aKeys.push_back(anIter.Key());
    }
    return aKeys;
  }

  //! Collects keys from the int-int map in iteration order.
  static std::vector<int> collectIntKeys(const IntIntMap& theMap)
  {
    std::vector<int> aKeys;
    for (IntIntMap::Iterator anIter(theMap); anIter.More(); anIter.Next())
    {
      aKeys.push_back(anIter.Key());
    }
    return aKeys;
  }
};

TEST_F(NCollection_OrderedDataMapTest, DefaultConstructor)
{
  IntStringMap aMap;

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_EQ(0, aMap.Extent());
}

TEST_F(NCollection_OrderedDataMapTest, BindAndFind)
{
  IntStringMap aMap;

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  EXPECT_FALSE(aMap.IsEmpty());
  EXPECT_EQ(3, aMap.Size());

  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  EXPECT_STREQ("Two", aMap.Find(2).ToCString());
  EXPECT_STREQ("Three", aMap.Find(3).ToCString());

  EXPECT_TRUE(aMap.IsBound(1));
  EXPECT_TRUE(aMap.IsBound(2));
  EXPECT_TRUE(aMap.IsBound(3));
  EXPECT_FALSE(aMap.IsBound(4));
}

TEST_F(NCollection_OrderedDataMapTest, BindPreservesOrder)
{
  IntStringMap aMap;

  aMap.Bind(30, "Thirty");
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  const std::vector<int> anExpected = {30, 10, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST_F(NCollection_OrderedDataMapTest, BindOverwritesValuePreservesOrder)
{
  IntStringMap aMap;

  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  // Re-bind key 2 with new value - should overwrite value but keep position
  aMap.Bind(2, "TWO");

  EXPECT_EQ(3, aMap.Size());
  EXPECT_STREQ("TWO", aMap.Find(2).ToCString());

  // Order should be unchanged
  const std::vector<int> anExpected = {1, 2, 3};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST_F(NCollection_OrderedDataMapTest, ChangeFind)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  EXPECT_STREQ("One", aMap.Find(1).ToCString());
  aMap.ChangeFind(1) = "Modified One";
  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, OperatorParens)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  // Const operator()
  const IntStringMap& aConstMap = aMap;
  EXPECT_STREQ("One", aConstMap(1).ToCString());

  // Non-const operator()
  aMap(1) = "Modified";
  EXPECT_STREQ("Modified", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, SeekAndChangeSeek)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  // Seek existing
  const TCollection_AsciiString* aPtr = aMap.Seek(1);
  ASSERT_NE(nullptr, aPtr);
  EXPECT_STREQ("One", aPtr->ToCString());

  // Seek non-existing
  EXPECT_EQ(nullptr, aMap.Seek(99));

  // ChangeSeek
  TCollection_AsciiString* aMutablePtr = aMap.ChangeSeek(1);
  ASSERT_NE(nullptr, aMutablePtr);
  *aMutablePtr = "Modified";
  EXPECT_STREQ("Modified", aMap.Find(1).ToCString());

  EXPECT_EQ(nullptr, aMap.ChangeSeek(99));
}

TEST_F(NCollection_OrderedDataMapTest, FindNonExisting)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  EXPECT_THROW(aMap.Find(2), Standard_NoSuchObject);
  EXPECT_THROW(aMap.ChangeFind(2), Standard_NoSuchObject);
}

TEST_F(NCollection_OrderedDataMapTest, FindWithCopy)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  TCollection_AsciiString aValue;
  EXPECT_TRUE(aMap.Find(1, aValue));
  EXPECT_STREQ("One", aValue.ToCString());

  EXPECT_FALSE(aMap.Find(99, aValue));
}

TEST_F(NCollection_OrderedDataMapTest, ContainedConst_Found)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  const IntStringMap& aConstMap = aMap;
  auto                aResult   = aConstMap.Contained(2);
  EXPECT_TRUE(aResult.has_value());
  EXPECT_EQ(2, aResult->first.get());
  EXPECT_STREQ("Two", aResult->second.get().ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, ContainedConst_NotFound)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  const IntStringMap& aConstMap = aMap;
  auto                aResult   = aConstMap.Contained(99);
  EXPECT_FALSE(aResult.has_value());
}

TEST_F(NCollection_OrderedDataMapTest, ContainedMutable)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  auto aResult = aMap.Contained(2);
  EXPECT_TRUE(aResult.has_value());
  EXPECT_EQ(2, aResult->first.get());
  EXPECT_STREQ("Two", aResult->second.get().ToCString());

  // Modify value through mutable reference
  aResult->second.get() = "Modified Two";
  EXPECT_STREQ("Modified Two", aMap.Find(2).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, ContainedEmpty)
{
  IntStringMap aMap;

  const IntStringMap& aConstMap = aMap;
  EXPECT_FALSE(aConstMap.Contained(1).has_value());
  EXPECT_FALSE(aMap.Contained(1).has_value());
}

TEST_F(NCollection_OrderedDataMapTest, UnBindOrder)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");
  aMap.Bind(4, "Four");

  // UnBind middle element
  EXPECT_TRUE(aMap.UnBind(2));
  EXPECT_EQ(3, aMap.Size());
  EXPECT_FALSE(aMap.IsBound(2));

  std::vector<int> anExpected = {1, 3, 4};
  EXPECT_EQ(anExpected, collectKeys(aMap));

  // UnBind first element
  EXPECT_TRUE(aMap.UnBind(1));
  anExpected = {3, 4};
  EXPECT_EQ(anExpected, collectKeys(aMap));

  // UnBind last element
  EXPECT_TRUE(aMap.UnBind(4));
  anExpected = {3};
  EXPECT_EQ(anExpected, collectKeys(aMap));

  // UnBind non-existent
  EXPECT_FALSE(aMap.UnBind(99));
}

TEST_F(NCollection_OrderedDataMapTest, Clear)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  aMap.Clear();
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
  EXPECT_FALSE(aMap.IsBound(1));
  EXPECT_FALSE(aMap.IsBound(2));
}

TEST_F(NCollection_OrderedDataMapTest, ClearAndReAdd)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  aMap.Clear();

  // Re-add in different order
  aMap.Bind(2, "Two");
  aMap.Bind(1, "One");

  const std::vector<int> anExpected = {2, 1};
  EXPECT_EQ(anExpected, collectKeys(aMap));
}

TEST_F(NCollection_OrderedDataMapTest, CopyPreservesOrder)
{
  IntStringMap aMap1;
  aMap1.Bind(30, "Thirty");
  aMap1.Bind(10, "Ten");
  aMap1.Bind(20, "Twenty");

  IntStringMap aMap2(aMap1);

  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_EQ(collectKeys(aMap1), collectKeys(aMap2));
  EXPECT_STREQ("Ten", aMap2.Find(10).ToCString());

  // Modify original - copy should be independent
  aMap1.ChangeFind(10) = "Modified Ten";
  EXPECT_STREQ("Ten", aMap2.Find(10).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, AssignPreservesOrder)
{
  IntStringMap aMap1;
  aMap1.Bind(30, "Thirty");
  aMap1.Bind(10, "Ten");
  aMap1.Bind(20, "Twenty");

  IntStringMap aMap2;
  aMap2.Bind(99, "Ninety-Nine");
  aMap2 = aMap1;

  EXPECT_EQ(collectKeys(aMap1), collectKeys(aMap2));
  EXPECT_FALSE(aMap2.IsBound(99));
}

TEST_F(NCollection_OrderedDataMapTest, MoveConstructor)
{
  IntStringMap aMap1;
  aMap1.Bind(10, "Ten");
  aMap1.Bind(20, "Twenty");
  aMap1.Bind(30, "Thirty");

  IntStringMap aMap2(std::move(aMap1));

  EXPECT_EQ(3, aMap2.Size());
  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap2));
  EXPECT_STREQ("Twenty", aMap2.Find(20).ToCString());

  EXPECT_TRUE(aMap1.IsEmpty());
}

TEST_F(NCollection_OrderedDataMapTest, MoveAssignment)
{
  IntStringMap aMap1;
  aMap1.Bind(10, "Ten");
  aMap1.Bind(20, "Twenty");
  aMap1.Bind(30, "Thirty");

  IntStringMap aMap2;
  aMap2 = std::move(aMap1);

  EXPECT_EQ(3, aMap2.Size());
  const std::vector<int> anExpected = {10, 20, 30};
  EXPECT_EQ(anExpected, collectKeys(aMap2));
}

TEST_F(NCollection_OrderedDataMapTest, Exchange)
{
  IntStringMap aMap1;
  aMap1.Bind(10, "Ten");
  aMap1.Bind(20, "Twenty");

  IntStringMap aMap2;
  aMap2.Bind(30, "Thirty");
  aMap2.Bind(40, "Forty");
  aMap2.Bind(50, "Fifty");

  aMap1.Exchange(aMap2);

  EXPECT_EQ(3, aMap1.Size());
  EXPECT_EQ(2, aMap2.Size());

  const std::vector<int> anExpected1 = {30, 40, 50};
  const std::vector<int> anExpected2 = {10, 20};
  EXPECT_EQ(anExpected1, collectKeys(aMap1));
  EXPECT_EQ(anExpected2, collectKeys(aMap2));

  EXPECT_STREQ("Thirty", aMap1.Find(30).ToCString());
  EXPECT_STREQ("Ten", aMap2.Find(10).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, ReSizePreservesOrder)
{
  IntIntMap aMap(10);

  for (int i = 0; i < 100; ++i)
  {
    aMap.Bind(i, i * 2);
  }

  EXPECT_EQ(100, aMap.Size());
  const std::vector<int> anOrderBefore = collectIntKeys(aMap);

  aMap.ReSize(200);

  EXPECT_EQ(100, aMap.Size());
  EXPECT_EQ(anOrderBefore, collectIntKeys(aMap));

  for (int i = 0; i < 100; ++i)
  {
    EXPECT_EQ(i * 2, aMap.Find(i));
  }
}

TEST_F(NCollection_OrderedDataMapTest, FirstLastKey)
{
  IntStringMap aMap;
  aMap.Bind(30, "Thirty");
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  EXPECT_EQ(30, aMap.First());
  EXPECT_EQ(20, aMap.Last());
}

TEST_F(NCollection_OrderedDataMapTest, FirstLastValue)
{
  IntStringMap aMap;
  aMap.Bind(30, "Thirty");
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  EXPECT_STREQ("Thirty", aMap.FirstValue().ToCString());
  EXPECT_STREQ("Twenty", aMap.LastValue().ToCString());

  // Modify through ChangeFirstValue/ChangeLastValue
  aMap.ChangeFirstValue() = "THIRTY";
  aMap.ChangeLastValue()  = "TWENTY";

  EXPECT_STREQ("THIRTY", aMap.FirstValue().ToCString());
  EXPECT_STREQ("TWENTY", aMap.LastValue().ToCString());
  EXPECT_STREQ("THIRTY", aMap.Find(30).ToCString());
  EXPECT_STREQ("TWENTY", aMap.Find(20).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, FirstLastEmpty)
{
  IntStringMap aMap;

  EXPECT_THROW(aMap.First(), Standard_NoSuchObject);
  EXPECT_THROW(aMap.Last(), Standard_NoSuchObject);
  EXPECT_THROW(aMap.FirstValue(), Standard_NoSuchObject);
  EXPECT_THROW(aMap.LastValue(), Standard_NoSuchObject);
  EXPECT_THROW(aMap.ChangeFirstValue(), Standard_NoSuchObject);
  EXPECT_THROW(aMap.ChangeLastValue(), Standard_NoSuchObject);
}

TEST_F(NCollection_OrderedDataMapTest, FirstLastAfterRemove)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  aMap.UnBind(1); // Remove first
  EXPECT_EQ(2, aMap.First());
  EXPECT_STREQ("Two", aMap.FirstValue().ToCString());

  aMap.UnBind(3); // Remove last
  EXPECT_EQ(2, aMap.Last());
  EXPECT_STREQ("Two", aMap.LastValue().ToCString());

  // Single element - first and last are the same
  EXPECT_EQ(aMap.First(), aMap.Last());
}

TEST_F(NCollection_OrderedDataMapTest, TryBind_NewKey)
{
  IntStringMap aMap;

  EXPECT_TRUE(aMap.TryBind(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, TryBind_ExistingKey)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  // TryBind should NOT replace existing value
  EXPECT_FALSE(aMap.TryBind(1, "New One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, TryBound_NewKey)
{
  IntStringMap aMap;

  TCollection_AsciiString& aRef = aMap.TryBound(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());

  aRef = "Modified One";
  EXPECT_STREQ("Modified One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, TryBound_ExistingKey)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  TCollection_AsciiString& aRef = aMap.TryBound(1, "New One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_OrderedDataMapTest, TryBindPreservesOrder)
{
  IntStringMap aMap;

  aMap.TryBind(30, "Thirty");
  aMap.TryBind(10, "Ten");
  aMap.TryBind(20, "Twenty");
  aMap.TryBind(10, "TEN"); // Should not change order or value

  const std::vector<int> anExpected = {30, 10, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
  EXPECT_STREQ("Ten", aMap.Find(10).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, Emplace_NewKey)
{
  IntStringMap aMap;

  EXPECT_TRUE(aMap.Emplace(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, Emplace_ExistingKey)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  EXPECT_FALSE(aMap.Emplace(1, "New One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("New One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, Emplaced)
{
  IntStringMap aMap;

  TCollection_AsciiString& aRef = aMap.Emplaced(1, "One");
  EXPECT_STREQ("One", aRef.ToCString());
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_OrderedDataMapTest, TryEmplace_NewKey)
{
  IntStringMap aMap;

  EXPECT_TRUE(aMap.TryEmplace(1, "One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, TryEmplace_ExistingKey)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");

  EXPECT_FALSE(aMap.TryEmplace(1, "New One"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_STREQ("One", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, TryEmplaced)
{
  IntStringMap aMap;

  TCollection_AsciiString& aRef1 = aMap.TryEmplaced(1, "One");
  EXPECT_STREQ("One", aRef1.ToCString());

  TCollection_AsciiString& aRef2 = aMap.TryEmplaced(1, "New One");
  EXPECT_STREQ("One", aRef2.ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, ItemsStructuredBindings)
{
  IntStringMap aMap;
  aMap.Bind(30, "Thirty");
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  // Verify Items() iterates in insertion order
  std::vector<int>         aKeys;
  std::vector<std::string> aValues;
  for (auto [aKey, aValue] : aMap.Items())
  {
    aKeys.push_back(aKey);
    aValues.push_back(aValue.ToCString());
  }

  const std::vector<int>         anExpectedKeys   = {30, 10, 20};
  const std::vector<std::string> anExpectedValues = {"Thirty", "Ten", "Twenty"};
  EXPECT_EQ(anExpectedKeys, aKeys);
  EXPECT_EQ(anExpectedValues, aValues);
}

TEST_F(NCollection_OrderedDataMapTest, ItemsModifyValue)
{
  IntStringMap aMap;
  aMap.Bind(1, "Original");

  for (auto [aKey, aValue] : aMap.Items())
  {
    (void)aKey;
    aValue = "Modified";
  }

  EXPECT_STREQ("Modified", aMap.Find(1).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, ConstItemsIteration)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");

  const IntStringMap& aConstMap = aMap;

  int aCount = 0;
  for (const auto& [aKey, aValue] : aConstMap.Items())
  {
    (void)aKey;
    (void)aValue;
    ++aCount;
  }

  EXPECT_EQ(2, aCount);
}

TEST_F(NCollection_OrderedDataMapTest, ItemsIteratorEquality)
{
  IntStringMap aMap;
  aMap.Bind(1, "One");
  aMap.Bind(2, "Two");
  aMap.Bind(3, "Three");

  auto aView = aMap.Items();

  EXPECT_EQ(aView.begin(), aView.begin());
  EXPECT_EQ(aView.end(), aView.end());
  EXPECT_NE(aView.begin(), aView.end());

  auto anIt1 = aView.begin();
  auto anIt2 = aView.begin();
  ++anIt2;
  EXPECT_NE(anIt1, anIt2);

  auto anIt3 = aView.begin();
  ++anIt3;
  EXPECT_EQ(anIt2, anIt3);
}

TEST_F(NCollection_OrderedDataMapTest, NativeIterator)
{
  IntStringMap aMap;
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");
  aMap.Bind(30, "Thirty");

  IntStringMap::Iterator anIter(aMap);
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(10, anIter.Key());
  EXPECT_STREQ("Ten", anIter.Value().ToCString());

  anIter.Next();
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(20, anIter.Key());
  EXPECT_STREQ("Twenty", anIter.Value().ToCString());

  // Test ChangeValue
  anIter.ChangeValue() = "TWENTY";
  EXPECT_STREQ("TWENTY", aMap.Find(20).ToCString());

  anIter.Next();
  EXPECT_TRUE(anIter.More());
  EXPECT_EQ(30, anIter.Key());

  anIter.Next();
  EXPECT_FALSE(anIter.More());
}

TEST_F(NCollection_OrderedDataMapTest, UnBindAndReAdd)
{
  IntStringMap aMap;
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");
  aMap.Bind(30, "Thirty");

  aMap.UnBind(20);
  aMap.Bind(40, "Forty");
  aMap.Bind(20, "TWENTY"); // Re-add at the end

  const std::vector<int> anExpected = {10, 30, 40, 20};
  EXPECT_EQ(anExpected, collectKeys(aMap));
  EXPECT_STREQ("TWENTY", aMap.Find(20).ToCString());
}

TEST_F(NCollection_OrderedDataMapTest, LargeMapOrderPreservation)
{
  const int NUM_ELEMENTS = 1000;
  IntIntMap aMap;

  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Bind(i, i * 2);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  int anIdx = 0;
  for (IntIntMap::Iterator anIter(aMap); anIter.More(); anIter.Next())
  {
    EXPECT_EQ(anIdx, anIter.Key());
    EXPECT_EQ(anIdx * 2, anIter.Value());
    ++anIdx;
  }
  EXPECT_EQ(NUM_ELEMENTS, anIdx);
}

TEST_F(NCollection_OrderedDataMapTest, STLBeginEnd)
{
  IntIntMap aMap;
  aMap.Bind(30, 300);
  aMap.Bind(10, 100);
  aMap.Bind(20, 200);

  // STL iterator gives values (not keys), in insertion order
  std::vector<int> aValues;
  for (auto it = aMap.cbegin(); it != aMap.cend(); ++it)
  {
    aValues.push_back(*it);
  }

  const std::vector<int> anExpected = {300, 100, 200};
  EXPECT_EQ(anExpected, aValues);
}

TEST_F(NCollection_OrderedDataMapTest, CustomStatefulHasher)
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

  NCollection_OrderedDataMap<int, TCollection_AsciiString, StatefulHasher> aMap(StatefulHasher(42),
                                                                                10);

  aMap.Bind(30, "Thirty");
  aMap.Bind(10, "Ten");
  aMap.Bind(20, "Twenty");

  EXPECT_EQ(3, aMap.Size());
  EXPECT_EQ(42, aMap.GetHasher().mySalt);
  EXPECT_STREQ("Ten", aMap.Find(10).ToCString());

  // Verify order
  std::vector<int> anOrder;
  for (NCollection_OrderedDataMap<int, TCollection_AsciiString, StatefulHasher>::Iterator anIter(
         aMap);
       anIter.More();
       anIter.Next())
  {
    anOrder.push_back(anIter.Key());
  }
  const std::vector<int> anExpected = {30, 10, 20};
  EXPECT_EQ(anExpected, anOrder);
}

TEST_F(NCollection_OrderedDataMapTest, AutoResize)
{
  IntIntMap aMap(2);

  std::vector<int> anExpectedKeys;
  for (int i = 0; i < 50; ++i)
  {
    aMap.Bind(i, i * 10);
    anExpectedKeys.push_back(i);
  }

  EXPECT_EQ(50, aMap.Size());
  EXPECT_EQ(anExpectedKeys, collectIntKeys(aMap));
}
