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

#include <NCollection_FlatMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>
#include <set>

//==================================================================================================
// NCollection_FlatMap Tests
//==================================================================================================

class NCollection_FlatMapTest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(NCollection_FlatMapTest, DefaultConstructor)
{
  NCollection_FlatMap<int> aMap;

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(0, aMap.Size());
}

TEST_F(NCollection_FlatMapTest, AddAndContains)
{
  NCollection_FlatMap<int> aMap;

  EXPECT_TRUE(aMap.Add(1));  // New key
  EXPECT_TRUE(aMap.Add(2));  // New key
  EXPECT_TRUE(aMap.Add(3));  // New key
  EXPECT_FALSE(aMap.Add(1)); // Duplicate

  EXPECT_EQ(3, aMap.Size());

  EXPECT_TRUE(aMap.Contains(1));
  EXPECT_TRUE(aMap.Contains(2));
  EXPECT_TRUE(aMap.Contains(3));
  EXPECT_FALSE(aMap.Contains(4));
}

TEST_F(NCollection_FlatMapTest, Remove)
{
  NCollection_FlatMap<int> aMap;

  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  EXPECT_TRUE(aMap.Remove(2));
  EXPECT_EQ(2, aMap.Size());
  EXPECT_FALSE(aMap.Contains(2));

  EXPECT_FALSE(aMap.Remove(4)); // Non-existent
  EXPECT_EQ(2, aMap.Size());
}

TEST_F(NCollection_FlatMapTest, Clear)
{
  NCollection_FlatMap<int> aMap;

  aMap.Add(1);
  aMap.Add(2);

  aMap.Clear();
  EXPECT_TRUE(aMap.IsEmpty());
}

TEST_F(NCollection_FlatMapTest, CopyConstructor)
{
  NCollection_FlatMap<int> aMap1;
  aMap1.Add(1);
  aMap1.Add(2);
  aMap1.Add(3);

  NCollection_FlatMap<int> aMap2(aMap1);

  EXPECT_EQ(aMap1.Size(), aMap2.Size());
  EXPECT_TRUE(aMap2.Contains(1));
  EXPECT_TRUE(aMap2.Contains(2));
  EXPECT_TRUE(aMap2.Contains(3));
}

TEST_F(NCollection_FlatMapTest, MoveConstructor)
{
  NCollection_FlatMap<int> aMap1;
  aMap1.Add(1);
  aMap1.Add(2);

  NCollection_FlatMap<int> aMap2(std::move(aMap1));

  EXPECT_EQ(2, aMap2.Size());
  EXPECT_TRUE(aMap1.IsEmpty());
}

TEST_F(NCollection_FlatMapTest, Iterator)
{
  NCollection_FlatMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  std::set<int>                      foundKeys;
  NCollection_FlatMap<int>::Iterator it(aMap);
  for (; it.More(); it.Next())
  {
    foundKeys.insert(it.Key());
  }

  EXPECT_EQ(3u, foundKeys.size());
  EXPECT_TRUE(foundKeys.find(10) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(20) != foundKeys.end());
  EXPECT_TRUE(foundKeys.find(30) != foundKeys.end());
}

TEST_F(NCollection_FlatMapTest, LargeDataSet)
{
  const int NUM_ELEMENTS = 10000;

  NCollection_FlatMap<int> aMap;

  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    aMap.Add(i);
  }

  EXPECT_EQ(NUM_ELEMENTS, aMap.Size());

  for (int i = 0; i < NUM_ELEMENTS; ++i)
  {
    EXPECT_TRUE(aMap.Contains(i)) << "Key " << i << " not found";
  }
}

TEST_F(NCollection_FlatMapTest, StringKeys)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;

  aMap.Add("Hello");
  aMap.Add("World");
  aMap.Add("Test");

  EXPECT_EQ(3, aMap.Size());
  EXPECT_TRUE(aMap.Contains("Hello"));
  EXPECT_TRUE(aMap.Contains("World"));
  EXPECT_TRUE(aMap.Contains("Test"));
  EXPECT_FALSE(aMap.Contains("NotFound"));
}

// Tests for Added method
TEST_F(NCollection_FlatMapTest, AddedNewKey)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;

  const TCollection_AsciiString& aRef = aMap.Added("Hello");
  EXPECT_TRUE(aRef.IsEqual("Hello"));
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatMapTest, AddedExistingKey)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;
  aMap.Add("Hello");

  const TCollection_AsciiString& aRef = aMap.Added("Hello");
  EXPECT_TRUE(aRef.IsEqual("Hello"));
  EXPECT_EQ(1, aMap.Size()); // Size unchanged
}

// Tests for Emplace method
TEST_F(NCollection_FlatMapTest, EmplaceNewKey)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;

  // Emplace with new key
  EXPECT_TRUE(aMap.Emplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_TRUE(aMap.Contains("Hello"));
}

TEST_F(NCollection_FlatMapTest, EmplaceExistingKey)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;
  aMap.Add("Hello");

  // Emplace on existing key - destroys and reconstructs
  EXPECT_FALSE(aMap.Emplace("Hello"));
  EXPECT_EQ(1, aMap.Size());
  EXPECT_TRUE(aMap.Contains("Hello"));
}

// Tests for Emplaced method
TEST_F(NCollection_FlatMapTest, EmplacedNewKey)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;

  const TCollection_AsciiString& aRef = aMap.Emplaced("World");
  EXPECT_TRUE(aRef.IsEqual("World"));
  EXPECT_EQ(1, aMap.Size());
}

TEST_F(NCollection_FlatMapTest, EmplacedExistingKey)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;
  aMap.Add("World");

  // Emplaced on existing - destroys and reconstructs
  const TCollection_AsciiString& aRef = aMap.Emplaced("World");
  EXPECT_TRUE(aRef.IsEqual("World"));
  EXPECT_EQ(1, aMap.Size());
}

// Tests for Seek method
TEST_F(NCollection_FlatMapTest, SeekFound)
{
  NCollection_FlatMap<int> aMap;
  aMap.Add(10);
  aMap.Add(20);
  aMap.Add(30);

  const int* pKey = aMap.Seek(10);
  ASSERT_NE(nullptr, pKey);
  EXPECT_EQ(10, *pKey);

  pKey = aMap.Seek(30);
  ASSERT_NE(nullptr, pKey);
  EXPECT_EQ(30, *pKey);
}

TEST_F(NCollection_FlatMapTest, SeekNotFound)
{
  NCollection_FlatMap<int> aMap;
  aMap.Add(10);

  const int* pKey = aMap.Seek(99);
  EXPECT_EQ(nullptr, pKey);

  // Seek on empty map
  NCollection_FlatMap<int> anEmptyMap;
  EXPECT_EQ(nullptr, anEmptyMap.Seek(10));
}

TEST_F(NCollection_FlatMapTest, ChangeSeekModify)
{
  NCollection_FlatMap<TCollection_AsciiString> aMap;
  aMap.Add("Hello");
  aMap.Add("World");

  TCollection_AsciiString* pKey = aMap.ChangeSeek("Hello");
  ASSERT_NE(nullptr, pKey);
  EXPECT_TRUE(pKey->IsEqual("Hello"));
}

TEST_F(NCollection_FlatMapTest, ChangeSeekNotFound)
{
  NCollection_FlatMap<int> aMap;
  aMap.Add(10);

  int* pKey = aMap.ChangeSeek(99);
  EXPECT_EQ(nullptr, pKey);
}

// Tests for hasher constructor
TEST_F(NCollection_FlatMapTest, HasherConstructorCopy)
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

  StatefulHasher                           aHasher(42);
  NCollection_FlatMap<int, StatefulHasher> aMap(aHasher, 10);

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

TEST_F(NCollection_FlatMapTest, HasherConstructorMove)
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

  NCollection_FlatMap<int, StatefulHasher> aMap(StatefulHasher(99), 10);

  aMap.Add(10);
  aMap.Add(20);

  EXPECT_EQ(2, aMap.Size());
  EXPECT_EQ(99, aMap.GetHasher().mySalt);
}

TEST_F(NCollection_FlatMapTest, CopyConstructorPreservesHasher)
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

  NCollection_FlatMap<int, StatefulHasher> aMap1(StatefulHasher(123), 10);
  aMap1.Add(1);
  aMap1.Add(2);

  // Copy construct
  NCollection_FlatMap<int, StatefulHasher> aMap2(aMap1);

  EXPECT_EQ(123, aMap2.GetHasher().mySalt);
  EXPECT_EQ(2, aMap2.Size());
  EXPECT_TRUE(aMap2.Contains(1));
  EXPECT_TRUE(aMap2.Contains(2));
}

// Test iterator equality
TEST_F(NCollection_FlatMapTest, IteratorEquality)
{
  NCollection_FlatMap<int> aMap;
  aMap.Add(1);
  aMap.Add(2);
  aMap.Add(3);

  // Test Iterator::IsEqual method directly
  NCollection_FlatMap<int>::Iterator anIt1(aMap);
  NCollection_FlatMap<int>::Iterator anIt2(aMap);
  NCollection_FlatMap<int>::Iterator anEndIt;

  // Two iterators at beginning should be equal
  EXPECT_TRUE(anIt1.IsEqual(anIt2));

  // Iterator at beginning should not equal end iterator
  EXPECT_FALSE(anIt1.IsEqual(anEndIt));

  // Two iterators at different positions must NOT be equal
  anIt2.Next();
  EXPECT_FALSE(anIt1.IsEqual(anIt2));

  // Iterators at same position should be equal
  NCollection_FlatMap<int>::Iterator anIt3(aMap);
  anIt3.Next();
  EXPECT_TRUE(anIt2.IsEqual(anIt3));

  // End iterators should be equal
  NCollection_FlatMap<int>::Iterator anEndIt2;
  EXPECT_TRUE(anEndIt.IsEqual(anEndIt2));
}
