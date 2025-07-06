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

#include <TCollection_ExtendedString.hxx>
#include <TCollection_AsciiString.hxx>

#include <gtest/gtest.h>

TEST(TCollection_ExtendedStringTest, DefaultConstructor)
{
  TCollection_ExtendedString aString;
  EXPECT_EQ(0, aString.Length());
  EXPECT_TRUE(aString.IsEmpty());
}

TEST(TCollection_ExtendedStringTest, ConstructorWithCString)
{
  TCollection_ExtendedString aString("Test String");
  EXPECT_EQ(11, aString.Length());
  EXPECT_FALSE(aString.IsEmpty());
}

TEST(TCollection_ExtendedStringTest, ConstructorWithChar)
{
  TCollection_ExtendedString aString('A');
  EXPECT_EQ(1, aString.Length());
  EXPECT_FALSE(aString.IsEmpty());
}

TEST(TCollection_ExtendedStringTest, ConstructorWithAsciiString)
{
  TCollection_AsciiString    asciiString("ASCII Test");
  TCollection_ExtendedString extendedString(asciiString);
  EXPECT_EQ(asciiString.Length(), extendedString.Length());
}

TEST(TCollection_ExtendedStringTest, CopyConstructor)
{
  TCollection_ExtendedString aString1("Original");
  TCollection_ExtendedString aString2(aString1);
  EXPECT_EQ(aString1.Length(), aString2.Length());
  EXPECT_TRUE(aString1.IsEqual(aString2));
}

TEST(TCollection_ExtendedStringTest, AssignmentOperator)
{
  TCollection_ExtendedString aString1("First");
  TCollection_ExtendedString aString2;
  aString2 = aString1;
  EXPECT_TRUE(aString1.IsEqual(aString2));

  // Change original to verify deep copy
  aString1 = TCollection_ExtendedString("Changed");
  EXPECT_FALSE(aString1.IsEqual(aString2));
}

TEST(TCollection_ExtendedStringTest, Concatenation)
{
  TCollection_ExtendedString aString1("Hello");
  TCollection_ExtendedString aString2(" World");
  TCollection_ExtendedString aString3 = aString1 + aString2;
  EXPECT_EQ(11, aString3.Length());

  // Convert to AsciiString for comparison
  TCollection_AsciiString asciiResult(aString3);
  EXPECT_STREQ("Hello World", asciiResult.ToCString());

  // Using += operator
  aString1 += aString2;
  EXPECT_TRUE(aString1.IsEqual(aString3));
}

TEST(TCollection_ExtendedStringTest, ConversionToFromAscii)
{
  // ASCII to Extended
  TCollection_AsciiString    asciiString("Test String");
  TCollection_ExtendedString extendedString(asciiString);

  // Extended back to ASCII
  TCollection_AsciiString convertedBack(extendedString);

  EXPECT_STREQ(asciiString.ToCString(), convertedBack.ToCString());
}

TEST(TCollection_ExtendedStringTest, Comparison)
{
  TCollection_ExtendedString aString1("Test");
  TCollection_ExtendedString aString2("Test");
  TCollection_ExtendedString aString3("Different");

  EXPECT_TRUE(aString1.IsEqual(aString2));
  EXPECT_TRUE(aString1 == aString2);
  EXPECT_FALSE(aString1.IsDifferent(aString2));
  EXPECT_FALSE(aString1 != aString2);

  EXPECT_FALSE(aString1.IsEqual(aString3));
  EXPECT_FALSE(aString1 == aString3);
  EXPECT_TRUE(aString1.IsDifferent(aString3));
  EXPECT_TRUE(aString1 != aString3);

  // Test IsLess and IsGreater
  TCollection_ExtendedString aStringA("A");
  TCollection_ExtendedString aStringZ("Z");

  EXPECT_TRUE(aStringA.IsLess(aStringZ));
  EXPECT_TRUE(aStringA < aStringZ);
  EXPECT_FALSE(aStringA.IsGreater(aStringZ));
  EXPECT_FALSE(aStringA > aStringZ);

  EXPECT_TRUE(aStringZ.IsGreater(aStringA));
  EXPECT_TRUE(aStringZ > aStringA);
  EXPECT_FALSE(aStringZ.IsLess(aStringA));
  EXPECT_FALSE(aStringZ < aStringA);
}

TEST(TCollection_ExtendedStringTest, UnicodeSupport)
{
  // Test with unicode characters
  const Standard_ExtCharacter unicodeChars[] = {0x0041, 0x00A9, 0x2122, 0x20AC, 0}; // A, copyright, trademark, euro
  TCollection_ExtendedString unicodeString(unicodeChars);

  EXPECT_EQ(4, unicodeString.Length());
  EXPECT_EQ(0x0041, unicodeString.Value(1)); // A
  EXPECT_EQ(0x00A9, unicodeString.Value(2)); // copyright
  EXPECT_EQ(0x2122, unicodeString.Value(3)); // trademark
  EXPECT_EQ(0x20AC, unicodeString.Value(4)); // euro
}

TEST(TCollection_ExtendedStringTest, HashCode)
{
  TCollection_ExtendedString aString1("Test");
  TCollection_ExtendedString aString2("Test");
  TCollection_ExtendedString aString3("Different");

  EXPECT_EQ(aString1.HashCode(), aString2.HashCode());
  EXPECT_NE(aString1.HashCode(), aString3.HashCode());
}

TEST(TCollection_ExtendedStringTest, Remove)
{
  TCollection_ExtendedString aString("Hello World");
  aString.Remove(6, 6); // Remove "World"

  TCollection_AsciiString asciiResult(aString);
  EXPECT_STREQ("Hello", asciiResult.ToCString());
}

TEST(TCollection_ExtendedStringTest, ToExtString)
{
  TCollection_ExtendedString aString("Test String");
  Standard_ExtString         extString = aString.ToExtString();

  // Check first few characters
  EXPECT_EQ('T', extString[0]);
  EXPECT_EQ('e', extString[1]);
  EXPECT_EQ('s', extString[2]);
  EXPECT_EQ('t', extString[3]);
}

TEST(TCollection_ExtendedStringTest, IsAscii)
{
  TCollection_ExtendedString asciiString("Simple ASCII");
  EXPECT_TRUE(asciiString.IsAscii());

  // Create a string with non-ASCII character
  Standard_ExtCharacter      unicodeChars[] = {'A', 0x20AC, 0}; // A, euro
  TCollection_ExtendedString unicodeString(unicodeChars);
  EXPECT_FALSE(unicodeString.IsAscii());
}

TEST(TCollection_ExtendedStringTest, Cat)
{
  TCollection_ExtendedString aString("Hello");

  // Cat with ExtendedString
  TCollection_ExtendedString result1 = aString.Cat(TCollection_ExtendedString(" World"));
  TCollection_AsciiString    asciiResult1(result1);
  EXPECT_STREQ("Hello World", asciiResult1.ToCString());

  // Cat with character
  TCollection_ExtendedString result2 = aString.Cat('!');
  TCollection_AsciiString    asciiResult2(result2);
  EXPECT_STREQ("Hello!", asciiResult2.ToCString());
}

TEST(TCollection_ExtendedStringTest, ChangeAll)
{
  TCollection_ExtendedString aString("Helloo Woorld");
  aString.ChangeAll('o', 'X');

  TCollection_AsciiString asciiResult(aString);
  EXPECT_STREQ("HellXX WXXrld", asciiResult.ToCString());
}
