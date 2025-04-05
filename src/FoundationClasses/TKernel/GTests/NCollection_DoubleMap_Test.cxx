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

#include <NCollection_DoubleMap.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

// Basic test types for the DoubleMap
typedef Standard_Integer Key1Type;
typedef Standard_Real    Key2Type;

// Custom key types for testing
class TestKey1
{
public:
  TestKey1(int id = 0, const char* name = "")
      : myId(id),
        myName(name)
  {
  }

  bool operator==(const TestKey1& other) const
  {
    return myId == other.myId && myName == other.myName;
  }

  int GetId() const { return myId; }

  const std::string& GetName() const { return myName; }

private:
  int         myId;
  std::string myName;
};

class TestKey2
{
public:
  TestKey2(double value = 0.0, const char* code = "")
      : myValue(value),
        myCode(code)
  {
  }

  bool operator==(const TestKey2& other) const
  {
    return fabs(myValue - other.myValue) < 1e-10 && myCode == other.myCode;
  }

  double GetValue() const { return myValue; }

  const std::string& GetCode() const { return myCode; }

private:
  double      myValue;
  std::string myCode;
};

namespace std
{
template <>
struct hash<TestKey1>
{
  size_t operator()(const TestKey1& key) const
  {
    return static_cast<size_t>(key.GetId() + std::hash<std::string>()(key.GetName()));
  }
};

template <>
struct hash<TestKey2>
{
  size_t operator()(const TestKey2& key) const
  {
    return static_cast<size_t>(key.GetValue() * 100 + std::hash<std::string>()(key.GetCode()));
  }
};
} // namespace std

TEST(NCollection_DoubleMapTest, DefaultConstructor)
{
  // Test default constructor
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
  EXPECT_EQ(aMap.Size(), 0);
}

TEST(NCollection_DoubleMapTest, CustomBuckets)
{
  // Test constructor with custom number of buckets
  NCollection_DoubleMap<Key1Type, Key2Type> aMap(100);
  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);
  EXPECT_EQ(aMap.Size(), 0);
}

TEST(NCollection_DoubleMapTest, BasicBindFind)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;

  // Test binding elements
  aMap.Bind(10, 1.0);
  aMap.Bind(20, 2.0);
  aMap.Bind(30, 3.0);

  EXPECT_EQ(aMap.Extent(), 3);

  // Test finding by Key1
  EXPECT_TRUE(aMap.IsBound1(10));
  EXPECT_TRUE(aMap.IsBound1(20));
  EXPECT_TRUE(aMap.IsBound1(30));
  EXPECT_FALSE(aMap.IsBound1(40));

  // Test finding by Key2
  EXPECT_TRUE(aMap.IsBound2(1.0));
  EXPECT_TRUE(aMap.IsBound2(2.0));
  EXPECT_TRUE(aMap.IsBound2(3.0));
  EXPECT_FALSE(aMap.IsBound2(4.0));

  // Test finding Key2 by Key1
  EXPECT_DOUBLE_EQ(aMap.Find1(10), 1.0);
  EXPECT_DOUBLE_EQ(aMap.Find1(20), 2.0);
  EXPECT_DOUBLE_EQ(aMap.Find1(30), 3.0);

  // Test finding Key1 by Key2
  EXPECT_EQ(aMap.Find2(1.0), 10);
  EXPECT_EQ(aMap.Find2(2.0), 20);
  EXPECT_EQ(aMap.Find2(3.0), 30);
}

TEST(NCollection_DoubleMapTest, BindWithDuplicateKeys)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;

  // First binding
  aMap.Bind(10, 1.0);
  EXPECT_EQ(aMap.Extent(), 1);
  EXPECT_DOUBLE_EQ(aMap.Find1(10), 1.0);

  // Try to bind a duplicate Key1 with a new Key2
  // This should fail silently in a production system, but we'll check that the map wasn't modified
  EXPECT_ANY_THROW(aMap.Bind(10, 2.0));
  EXPECT_EQ(aMap.Extent(), 1);           // Size should remain 1
  EXPECT_DOUBLE_EQ(aMap.Find1(10), 1.0); // Value should remain unchanged
  EXPECT_FALSE(aMap.IsBound2(2.0));      // The new Key2 should not be bound

  // Try to bind a new Key1 with duplicate Key2
  EXPECT_ANY_THROW(aMap.Bind(20, 1.0));
  EXPECT_EQ(aMap.Extent(), 1);     // Size should remain 1
  EXPECT_FALSE(aMap.IsBound1(20)); // The new Key1 should not be bound
}

TEST(NCollection_DoubleMapTest, UnbindTest)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;

  // Add elements
  aMap.Bind(10, 1.0);
  aMap.Bind(20, 2.0);
  aMap.Bind(30, 3.0);

  EXPECT_EQ(aMap.Extent(), 3);

  // Test UnBind1
  aMap.UnBind1(20);
  EXPECT_EQ(aMap.Extent(), 2);
  EXPECT_FALSE(aMap.IsBound1(20));
  EXPECT_FALSE(aMap.IsBound2(2.0));

  // Test UnBind2
  aMap.UnBind2(3.0);
  EXPECT_EQ(aMap.Extent(), 1);
  EXPECT_FALSE(aMap.IsBound1(30));
  EXPECT_FALSE(aMap.IsBound2(3.0));

  // Try to unbind non-existent keys
  aMap.UnBind1(40); // Should have no effect
  EXPECT_EQ(aMap.Extent(), 1);

  aMap.UnBind2(4.0); // Should have no effect
  EXPECT_EQ(aMap.Extent(), 1);
}

TEST(NCollection_DoubleMapTest, Clear)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;

  // Add elements
  aMap.Bind(10, 1.0);
  aMap.Bind(20, 2.0);
  aMap.Bind(30, 3.0);

  EXPECT_EQ(aMap.Extent(), 3);

  // Clear the map
  aMap.Clear();

  EXPECT_TRUE(aMap.IsEmpty());
  EXPECT_EQ(aMap.Extent(), 0);

  // Ensure we can add elements again
  aMap.Bind(40, 4.0);
  EXPECT_EQ(aMap.Extent(), 1);
}

TEST(NCollection_DoubleMapTest, SeekTests)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;

  // Add elements
  aMap.Bind(10, 1.0);
  aMap.Bind(20, 2.0);

  // Test Seek1
  const Key2Type* key2Ptr = aMap.Seek1(10);
  EXPECT_TRUE(key2Ptr != nullptr);
  EXPECT_DOUBLE_EQ(*key2Ptr, 1.0);

  // Test Seek2
  const Key1Type* key1Ptr = aMap.Seek2(2.0);
  EXPECT_TRUE(key1Ptr != nullptr);
  EXPECT_EQ(*key1Ptr, 20);

  // Test seeking non-existent keys
  EXPECT_TRUE(aMap.Seek1(30) == nullptr);
  EXPECT_TRUE(aMap.Seek2(3.0) == nullptr);
}

TEST(NCollection_DoubleMapTest, CopyConstructor)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap1;

  // Add elements to first map
  aMap1.Bind(10, 1.0);
  aMap1.Bind(20, 2.0);
  aMap1.Bind(30, 3.0);

  // Create a copy
  NCollection_DoubleMap<Key1Type, Key2Type> aMap2(aMap1);

  // Check that copy has the same elements
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_TRUE(aMap2.IsBound1(10));
  EXPECT_TRUE(aMap2.IsBound1(20));
  EXPECT_TRUE(aMap2.IsBound1(30));
  EXPECT_TRUE(aMap2.IsBound2(1.0));
  EXPECT_TRUE(aMap2.IsBound2(2.0));
  EXPECT_TRUE(aMap2.IsBound2(3.0));

  // Modify original to ensure deep copy
  aMap1.Bind(40, 4.0);
  EXPECT_EQ(aMap1.Extent(), 4);
  EXPECT_EQ(aMap2.Extent(), 3);
  EXPECT_FALSE(aMap2.IsBound1(40));
  EXPECT_FALSE(aMap2.IsBound2(4.0));
}

TEST(NCollection_DoubleMapTest, AssignmentOperator)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap1;
  NCollection_DoubleMap<Key1Type, Key2Type> aMap2;

  // Add elements to first map
  aMap1.Bind(10, 1.0);
  aMap1.Bind(20, 2.0);

  // Add different elements to second map
  aMap2.Bind(30, 3.0);
  aMap2.Bind(40, 4.0);
  aMap2.Bind(50, 5.0);

  // Assign first to second
  aMap2 = aMap1;

  // Check that second map now matches first
  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.IsBound1(10));
  EXPECT_TRUE(aMap2.IsBound1(20));
  EXPECT_FALSE(aMap2.IsBound1(30));
  EXPECT_FALSE(aMap2.IsBound1(40));
  EXPECT_FALSE(aMap2.IsBound1(50));

  EXPECT_TRUE(aMap2.IsBound2(1.0));
  EXPECT_TRUE(aMap2.IsBound2(2.0));
  EXPECT_FALSE(aMap2.IsBound2(3.0));
  EXPECT_FALSE(aMap2.IsBound2(4.0));
  EXPECT_FALSE(aMap2.IsBound2(5.0));
}

TEST(NCollection_DoubleMapTest, ReSize)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap(3); // Start with small bucket count

  // Add many elements to trigger resize
  for (Standard_Integer i = 1; i <= 100; ++i)
  {
    aMap.Bind(i, static_cast<Standard_Real>(i) / 10.0);
  }

  // Verify all elements are present
  EXPECT_EQ(aMap.Extent(), 100);
  for (Standard_Integer i = 1; i <= 100; ++i)
  {
    EXPECT_TRUE(aMap.IsBound1(i));
    EXPECT_TRUE(aMap.IsBound2(static_cast<Standard_Real>(i) / 10.0));
    EXPECT_DOUBLE_EQ(aMap.Find1(i), static_cast<Standard_Real>(i) / 10.0);
    EXPECT_EQ(aMap.Find2(static_cast<Standard_Real>(i) / 10.0), i);
  }
}

TEST(NCollection_DoubleMapTest, Exchange)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap1;
  NCollection_DoubleMap<Key1Type, Key2Type> aMap2;

  // Add elements to first map
  aMap1.Bind(10, 1.0);
  aMap1.Bind(20, 2.0);

  // Add different elements to second map
  aMap2.Bind(30, 3.0);
  aMap2.Bind(40, 4.0);
  aMap2.Bind(50, 5.0);

  // Exchange maps
  aMap1.Exchange(aMap2);

  // Check that maps are exchanged
  EXPECT_EQ(aMap1.Extent(), 3);
  EXPECT_TRUE(aMap1.IsBound1(30));
  EXPECT_TRUE(aMap1.IsBound1(40));
  EXPECT_TRUE(aMap1.IsBound1(50));
  EXPECT_TRUE(aMap1.IsBound2(3.0));
  EXPECT_TRUE(aMap1.IsBound2(4.0));
  EXPECT_TRUE(aMap1.IsBound2(5.0));

  EXPECT_EQ(aMap2.Extent(), 2);
  EXPECT_TRUE(aMap2.IsBound1(10));
  EXPECT_TRUE(aMap2.IsBound1(20));
  EXPECT_TRUE(aMap2.IsBound2(1.0));
  EXPECT_TRUE(aMap2.IsBound2(2.0));
}

TEST(NCollection_DoubleMapTest, Iterator)
{
  NCollection_DoubleMap<Key1Type, Key2Type> aMap;

  // Add elements
  aMap.Bind(10, 1.0);
  aMap.Bind(20, 2.0);
  aMap.Bind(30, 3.0);

  // Use iterator to check all elements
  Standard_Boolean found10 = Standard_False;
  Standard_Boolean found20 = Standard_False;
  Standard_Boolean found30 = Standard_False;
  Standard_Size    count   = 0;

  for (NCollection_DoubleMap<Key1Type, Key2Type>::Iterator it(aMap); it.More(); it.Next(), ++count)
  {
    const Key1Type& key1 = it.Key1();
    const Key2Type& key2 = it.Key2();

    if (key1 == 10 && key2 == 1.0)
      found10 = Standard_True;
    else if (key1 == 20 && key2 == 2.0)
      found20 = Standard_True;
    else if (key1 == 30 && key2 == 3.0)
      found30 = Standard_True;
  }

  EXPECT_EQ(count, 3);
  EXPECT_TRUE(found10);
  EXPECT_TRUE(found20);
  EXPECT_TRUE(found30);
}

TEST(NCollection_DoubleMapTest, StringKeys)
{
  // Test with string keys
  NCollection_DoubleMap<TCollection_AsciiString, TCollection_AsciiString> aStringMap;

  // Add string pairs
  aStringMap.Bind(TCollection_AsciiString("Key1"), TCollection_AsciiString("Value1"));
  aStringMap.Bind(TCollection_AsciiString("Key2"), TCollection_AsciiString("Value2"));
  aStringMap.Bind(TCollection_AsciiString("Key3"), TCollection_AsciiString("Value3"));

  EXPECT_EQ(aStringMap.Extent(), 3);

  // Check key-value bindings
  EXPECT_TRUE(aStringMap.IsBound1(TCollection_AsciiString("Key1")));
  EXPECT_TRUE(aStringMap.IsBound1(TCollection_AsciiString("Key2")));
  EXPECT_TRUE(aStringMap.IsBound1(TCollection_AsciiString("Key3")));

  EXPECT_TRUE(aStringMap.IsBound2(TCollection_AsciiString("Value1")));
  EXPECT_TRUE(aStringMap.IsBound2(TCollection_AsciiString("Value2")));
  EXPECT_TRUE(aStringMap.IsBound2(TCollection_AsciiString("Value3")));

  // Test finding values by keys
  EXPECT_TRUE(aStringMap.Find1(TCollection_AsciiString("Key1")).IsEqual("Value1"));
  EXPECT_TRUE(aStringMap.Find1(TCollection_AsciiString("Key2")).IsEqual("Value2"));
  EXPECT_TRUE(aStringMap.Find1(TCollection_AsciiString("Key3")).IsEqual("Value3"));

  // Test finding keys by values
  EXPECT_TRUE(aStringMap.Find2(TCollection_AsciiString("Value1")).IsEqual("Key1"));
  EXPECT_TRUE(aStringMap.Find2(TCollection_AsciiString("Value2")).IsEqual("Key2"));
  EXPECT_TRUE(aStringMap.Find2(TCollection_AsciiString("Value3")).IsEqual("Key3"));
}

TEST(NCollection_DoubleMapTest, ComplexKeys)
{
  // Create map with custom key types and hashers
  NCollection_DoubleMap<TestKey1, TestKey2> aComplexMap;

  // Add complex key pairs
  TestKey1 key1_1(1, "One");
  TestKey1 key1_2(2, "Two");
  TestKey1 key1_3(3, "Three");

  TestKey2 key2_1(1.1, "A");
  TestKey2 key2_2(2.2, "B");
  TestKey2 key2_3(3.3, "C");

  aComplexMap.Bind(key1_1, key2_1);
  aComplexMap.Bind(key1_2, key2_2);
  aComplexMap.Bind(key1_3, key2_3);

  EXPECT_EQ(aComplexMap.Extent(), 3);

  // Test IsBound for both key types
  EXPECT_TRUE(aComplexMap.IsBound1(TestKey1(1, "One")));
  EXPECT_TRUE(aComplexMap.IsBound1(TestKey1(2, "Two")));
  EXPECT_TRUE(aComplexMap.IsBound1(TestKey1(3, "Three")));

  EXPECT_TRUE(aComplexMap.IsBound2(TestKey2(1.1, "A")));
  EXPECT_TRUE(aComplexMap.IsBound2(TestKey2(2.2, "B")));
  EXPECT_TRUE(aComplexMap.IsBound2(TestKey2(3.3, "C")));

  // Test finding second key by first key
  const TestKey2& foundKey2 = aComplexMap.Find1(key1_2);
  EXPECT_DOUBLE_EQ(foundKey2.GetValue(), 2.2);
  EXPECT_EQ(foundKey2.GetCode(), "B");

  // Test finding first key by second key
  const TestKey1& foundKey1 = aComplexMap.Find2(key2_3);
  EXPECT_EQ(foundKey1.GetId(), 3);
  EXPECT_EQ(foundKey1.GetName(), "Three");
}
