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

#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

TEST(TCollection_AsciiStringTest, DefaultConstructor)
{
  TCollection_AsciiString aString;
  EXPECT_EQ(0, aString.Length());
  EXPECT_TRUE(aString.IsEmpty());
}

TEST(TCollection_AsciiStringTest, ConstructorWithCString)
{
  TCollection_AsciiString aString("Test String");
  EXPECT_EQ(11, aString.Length());
  EXPECT_FALSE(aString.IsEmpty());
  EXPECT_STREQ("Test String", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, ConstructorWithChar)
{
  TCollection_AsciiString aString('A');
  EXPECT_EQ(1, aString.Length());
  EXPECT_FALSE(aString.IsEmpty());
  EXPECT_STREQ("A", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, CopyConstructor)
{
  TCollection_AsciiString aString1("Original");
  TCollection_AsciiString aString2(aString1);
  EXPECT_STREQ(aString1.ToCString(), aString2.ToCString());
  EXPECT_EQ(aString1.Length(), aString2.Length());
}

TEST(TCollection_AsciiStringTest, AssignmentOperator)
{
  TCollection_AsciiString aString1("First");
  TCollection_AsciiString aString2;
  aString2 = aString1;
  EXPECT_STREQ(aString1.ToCString(), aString2.ToCString());

  // Change original to verify deep copy
  aString1 = "Changed";
  EXPECT_STRNE(aString1.ToCString(), aString2.ToCString());
  EXPECT_STREQ("First", aString2.ToCString());
}

TEST(TCollection_AsciiStringTest, Concatenation)
{
  TCollection_AsciiString aString1("Hello");
  TCollection_AsciiString aString2(" World");
  TCollection_AsciiString aString3 = aString1 + aString2;
  EXPECT_STREQ("Hello World", aString3.ToCString());

  // Using += operator
  aString1 += aString2;
  EXPECT_STREQ("Hello World", aString1.ToCString());

  // Concatenate with char
  aString1 += '!';
  EXPECT_STREQ("Hello World!", aString1.ToCString());

  // Concatenate with C string
  aString1 += " Test";
  EXPECT_STREQ("Hello World! Test", aString1.ToCString());
}

TEST(TCollection_AsciiStringTest, SubString)
{
  TCollection_AsciiString aString("ABCDEFGHIJKLMNOPQRSTUVWXYZ");
  TCollection_AsciiString subString = aString.SubString(3, 8);
  EXPECT_STREQ("CDEFGH", subString.ToCString());
}

TEST(TCollection_AsciiStringTest, ChangeCase)
{
  TCollection_AsciiString aString("Hello World");

  // Test to upper case
  TCollection_AsciiString upperString = aString;
  upperString.UpperCase();
  EXPECT_STREQ("HELLO WORLD", upperString.ToCString());

  // Test to lower case
  TCollection_AsciiString lowerString = aString;
  lowerString.LowerCase();
  EXPECT_STREQ("hello world", lowerString.ToCString());

  // Test capitalize
  TCollection_AsciiString capitalizeString = lowerString;
  capitalizeString.Capitalize();
  EXPECT_STREQ("Hello world", capitalizeString.ToCString());
}

TEST(TCollection_AsciiStringTest, SearchAndLocation)
{
  TCollection_AsciiString aString("This is a test string for testing search functions");

  // Test Search
  EXPECT_EQ(11, aString.Search("test"));
  EXPECT_EQ(-1, aString.Search("nonexistent"));

  // Test SearchFromEnd
  EXPECT_EQ(27, aString.SearchFromEnd("test"));

  // Test Location
  EXPECT_EQ(3, aString.Location(TCollection_AsciiString("i"), 1, aString.Length()));
  EXPECT_EQ(6, aString.Location(TCollection_AsciiString("i"), 4, aString.Length()));
}

TEST(TCollection_AsciiStringTest, RemoveAndInsert)
{
  TCollection_AsciiString aString("Hello World");

  // Test Remove
  aString.Remove(6, 6);
  EXPECT_STREQ("Hello", aString.ToCString());

  // Test Insert
  aString.Insert(6, " Universe");
  EXPECT_STREQ("Hello Universe", aString.ToCString());

  // Test RemoveAll
  aString.RemoveAll('e');
  EXPECT_STREQ("Hllo Univrs", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, Comparison)
{
  TCollection_AsciiString aString1("Test");
  TCollection_AsciiString aString2("Test");
  TCollection_AsciiString aString3("Different");

  EXPECT_TRUE(aString1.IsEqual(aString2));
  EXPECT_TRUE(aString1 == aString2);
  EXPECT_FALSE(aString1.IsDifferent(aString2));
  EXPECT_FALSE(aString1 != aString2);

  EXPECT_FALSE(aString1.IsEqual(aString3));
  EXPECT_FALSE(aString1 == aString3);
  EXPECT_TRUE(aString1.IsDifferent(aString3));
  EXPECT_TRUE(aString1 != aString3);

  // Testing with C strings
  EXPECT_TRUE(aString1.IsEqual("Test"));
  EXPECT_TRUE(aString1 == "Test");
  EXPECT_FALSE(aString1.IsEqual("Different"));
  EXPECT_FALSE(aString1 == "Different");

  // Test IsLess and IsGreater
  TCollection_AsciiString aStringA("A");
  TCollection_AsciiString aStringZ("Z");

  EXPECT_TRUE(aStringA.IsLess(aStringZ));
  EXPECT_TRUE(aStringA < aStringZ);
  EXPECT_FALSE(aStringA.IsGreater(aStringZ));
  EXPECT_FALSE(aStringA > aStringZ);

  EXPECT_TRUE(aStringZ.IsGreater(aStringA));
  EXPECT_TRUE(aStringZ > aStringA);
  EXPECT_FALSE(aStringZ.IsLess(aStringA));
  EXPECT_FALSE(aStringZ < aStringA);
}

TEST(TCollection_AsciiStringTest, Token)
{
  TCollection_AsciiString aString("This is a test string");

  EXPECT_STREQ("This", aString.Token().ToCString());
  EXPECT_STREQ("is", aString.Token(" ", 2).ToCString());
  EXPECT_STREQ("a", aString.Token(" ", 3).ToCString());
  EXPECT_STREQ("test", aString.Token(" ", 4).ToCString());
  EXPECT_STREQ("string", aString.Token(" ", 5).ToCString());
  EXPECT_STREQ("", aString.Token(" ", 6).ToCString()); // No 6th token

  // Test with different separators
  TCollection_AsciiString aStringWithSeps("one,two;three:four");
  EXPECT_STREQ("one", aStringWithSeps.Token(",;:", 1).ToCString());
  EXPECT_STREQ("two", aStringWithSeps.Token(",;:", 2).ToCString());
  EXPECT_STREQ("three", aStringWithSeps.Token(",;:", 3).ToCString());
  EXPECT_STREQ("four", aStringWithSeps.Token(",;:", 4).ToCString());
}

TEST(TCollection_AsciiStringTest, ValueConversion)
{
  // Test integer conversion
  TCollection_AsciiString intString("12345");
  EXPECT_EQ(12345, intString.IntegerValue());
  EXPECT_TRUE(intString.IsIntegerValue());

  // Test real value conversion
  TCollection_AsciiString realString("123.456");
  EXPECT_DOUBLE_EQ(123.456, realString.RealValue());
  EXPECT_TRUE(realString.IsRealValue());

  // Test invalid conversions
  TCollection_AsciiString invalidString("not a number");
  EXPECT_FALSE(invalidString.IsIntegerValue());
  EXPECT_FALSE(invalidString.IsRealValue());
}

TEST(TCollection_AsciiStringTest, HashCode)
{
  TCollection_AsciiString aString1("Test");
  TCollection_AsciiString aString2("Test");
  TCollection_AsciiString aString3("Different");

  EXPECT_EQ(aString1.HashCode(), aString2.HashCode());
  EXPECT_NE(aString1.HashCode(), aString3.HashCode());
}

TEST(TCollection_AsciiStringTest, Split)
{
  TCollection_AsciiString aString("abcdefghij");
  TCollection_AsciiString remainder = aString.Split(5);

  EXPECT_STREQ("abcde", aString.ToCString());
  EXPECT_STREQ("fghij", remainder.ToCString());
}
