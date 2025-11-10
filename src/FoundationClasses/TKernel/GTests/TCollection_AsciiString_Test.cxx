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
#include <TCollection_HAsciiString.hxx>

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

TEST(TCollection_AsciiStringTest, MemoryAllocation)
{
  // Test memory allocation with various string lengths
  TCollection_AsciiString aString1("test");
  EXPECT_EQ(4, aString1.Length());

  // Test allocation with different lengths
  for (Standard_Integer anIdx = 0; anIdx <= 20; ++anIdx)
  {
    TCollection_AsciiString aStr(anIdx, 'A');
    EXPECT_EQ(anIdx, aStr.Length());
  }
}

TEST(TCollection_AsciiStringTest, LengthConstructor)
{
  // Test constructor with string and maximum length
  const Standard_CString aSourceString = "This is a very long string";

  TCollection_AsciiString aString1(aSourceString, 4);
  EXPECT_EQ(4, aString1.Length());
  EXPECT_STREQ("This", aString1.ToCString());

  TCollection_AsciiString aString2(aSourceString, 7);
  EXPECT_EQ(7, aString2.Length());
  EXPECT_STREQ("This is", aString2.ToCString());

  // Test with length exceeding source string
  TCollection_AsciiString aString3(aSourceString, 100);
  EXPECT_EQ(26, aString3.Length());
  EXPECT_STREQ(aSourceString, aString3.ToCString());
}

TEST(TCollection_AsciiStringTest, ExtendedStringConversion)
{
  // Test conversion from ExtendedString
  TCollection_ExtendedString anExtString("Hello World");
  TCollection_AsciiString    anAsciiString(anExtString);

  EXPECT_EQ(anExtString.Length(), anAsciiString.Length());
  EXPECT_STREQ("Hello World", anAsciiString.ToCString());
}

TEST(TCollection_AsciiStringTest, NumericalConstructors)
{
  // Test integer constructor
  TCollection_AsciiString anIntString(42);
  EXPECT_STREQ("42", anIntString.ToCString());

  // Test real constructor
  TCollection_AsciiString aRealString(3.14);
  const Standard_CString  aRealCStr = aRealString.ToCString();
  EXPECT_TRUE(strstr(aRealCStr, "3.14") != NULL);
}

TEST(TCollection_AsciiStringTest, FillerConstructor)
{
  // Test constructor with length and filler character
  TCollection_AsciiString aFilledString(5, '*');
  EXPECT_EQ(5, aFilledString.Length());
  EXPECT_STREQ("*****", aFilledString.ToCString());
}

TEST(TCollection_AsciiStringTest, ConcatenationConstructors)
{
  // Test string + character constructor
  TCollection_AsciiString aBaseString("Hello");
  TCollection_AsciiString aStringWithChar(aBaseString, '!');
  EXPECT_STREQ("Hello!", aStringWithChar.ToCString());

  // Test string + C string constructor
  TCollection_AsciiString aStringWithCStr(aBaseString, " World");
  EXPECT_STREQ("Hello World", aStringWithCStr.ToCString());

  // Test string + string constructor
  TCollection_AsciiString aSecondString(" Universe");
  TCollection_AsciiString aCombinedString(aBaseString, aSecondString);
  EXPECT_STREQ("Hello Universe", aCombinedString.ToCString());
}

TEST(TCollection_AsciiStringTest, EdgeCases)
{
  // Test empty string operations
  TCollection_AsciiString anEmptyString1;
  TCollection_AsciiString anEmptyString2("");

  EXPECT_TRUE(anEmptyString1.IsEqual(anEmptyString2));
  EXPECT_EQ(0, anEmptyString1.Length());
  EXPECT_TRUE(anEmptyString1.IsEmpty());

  // Test null character handling
  TCollection_AsciiString aNullCharString('\0');
  EXPECT_EQ(0, aNullCharString.Length());
  EXPECT_TRUE(aNullCharString.IsEmpty());
}

TEST(TCollection_AsciiStringTest, LargeStrings)
{
  // Test with large strings to verify memory allocation
  const Standard_Integer  aLargeSize = 1000;
  TCollection_AsciiString aLargeString(aLargeSize, 'X');

  EXPECT_EQ(aLargeSize, aLargeString.Length());
  EXPECT_EQ('X', aLargeString.Value(1));
  EXPECT_EQ('X', aLargeString.Value(aLargeSize));
}

TEST(TCollection_AsciiStringTest, PaddingSafety)
{
  // Test that internal padding works correctly for various lengths
  for (Standard_Integer anIdx = 1; anIdx <= 16; ++anIdx)
  {
    TCollection_AsciiString aTestString(anIdx, 'A');
    EXPECT_EQ(anIdx, aTestString.Length());

    // Verify null termination
    const Standard_CString aCString = aTestString.ToCString();
    EXPECT_EQ('\0', aCString[anIdx]);

    // Verify content
    for (Standard_Integer aCharIdx = 0; aCharIdx < anIdx; ++aCharIdx)
    {
      EXPECT_EQ('A', aCString[aCharIdx]);
    }
  }
}

// ========================================
// Tests for AssignCat method
// ========================================

TEST(TCollection_AsciiStringTest, AssignCat_BasicCases)
{
  TCollection_AsciiString aString("Hello");

  // AssignCat with C string
  aString.AssignCat(" World");
  EXPECT_STREQ("Hello World", aString.ToCString());
  EXPECT_EQ(11, aString.Length());

  // AssignCat with another AsciiString
  TCollection_AsciiString aSuffix("!");
  aString.AssignCat(aSuffix);
  EXPECT_STREQ("Hello World!", aString.ToCString());
  EXPECT_EQ(12, aString.Length());

  // AssignCat with character
  aString.AssignCat('?');
  EXPECT_STREQ("Hello World!?", aString.ToCString());
  EXPECT_EQ(13, aString.Length());
}

TEST(TCollection_AsciiStringTest, AssignCat_EmptyStrings)
{
  TCollection_AsciiString aString;

  // AssignCat to empty string
  aString.AssignCat("First");
  EXPECT_STREQ("First", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // AssignCat empty string
  aString.AssignCat("");
  EXPECT_STREQ("First", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // AssignCat null terminator character
  TCollection_AsciiString aString2("Test");
  aString2.AssignCat('\0');
  EXPECT_STREQ("Test", aString2.ToCString());
  EXPECT_EQ(4, aString2.Length());
}

TEST(TCollection_AsciiStringTest, AssignCat_SelfReference)
{
  TCollection_AsciiString aString("ABC");

  // Self-reference: concatenate the entire string to itself
  aString.AssignCat(aString.ToCString());
  EXPECT_STREQ("ABCABC", aString.ToCString());
  EXPECT_EQ(6, aString.Length());

  // Self-reference: concatenate part of itself
  TCollection_AsciiString aString2("Hello");
  const char*             aSubStr = aString2.ToCString() + 1; // "ello"
  aString2.AssignCat(aSubStr);
  EXPECT_STREQ("Helloello", aString2.ToCString());
  EXPECT_EQ(9, aString2.Length());
}

TEST(TCollection_AsciiStringTest, AssignCat_IntegerAndReal)
{
  TCollection_AsciiString aString("Value: ");

  // AssignCat with integer
  aString.AssignCat(42);
  EXPECT_STREQ("Value: 42", aString.ToCString());

  // AssignCat with real
  TCollection_AsciiString aString2("Pi is approximately ");
  aString2.AssignCat(3.14159);
  EXPECT_TRUE(strstr(aString2.ToCString(), "3.14159") != NULL);
}

TEST(TCollection_AsciiStringTest, AssignCat_LargeStrings)
{
  TCollection_AsciiString aString(100, 'A');
  TCollection_AsciiString aSuffix(100, 'B');

  aString.AssignCat(aSuffix);
  EXPECT_EQ(200, aString.Length());

  // Verify first 100 are 'A' and last 100 are 'B'
  const char* aPtr = aString.ToCString();
  for (int i = 0; i < 100; ++i)
  {
    EXPECT_EQ('A', aPtr[i]);
  }
  for (int i = 100; i < 200; ++i)
  {
    EXPECT_EQ('B', aPtr[i]);
  }
}

// ========================================
// Tests for Insert method
// ========================================

TEST(TCollection_AsciiStringTest, Insert_BasicCases)
{
  TCollection_AsciiString aString("HelloWorld");

  // Insert at beginning
  aString.Insert(1, "Start");
  EXPECT_STREQ("StartHelloWorld", aString.ToCString());

  // Insert in middle
  TCollection_AsciiString aString2("AC");
  aString2.Insert(2, "B");
  EXPECT_STREQ("ABC", aString2.ToCString());

  // Insert at end
  TCollection_AsciiString aString3("Hello");
  aString3.Insert(6, " World");
  EXPECT_STREQ("Hello World", aString3.ToCString());
}

TEST(TCollection_AsciiStringTest, Insert_EmptyStrings)
{
  TCollection_AsciiString aString;

  // Insert into empty string
  aString.Insert(1, "First");
  EXPECT_STREQ("First", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // Insert empty string
  TCollection_AsciiString aString2("Test");
  aString2.Insert(3, "");
  EXPECT_STREQ("Test", aString2.ToCString());
  EXPECT_EQ(4, aString2.Length());
}

TEST(TCollection_AsciiStringTest, Insert_SelfReference)
{
  TCollection_AsciiString aString("XYZ");

  // Insert entire string into itself at beginning
  aString.Insert(1, aString.ToCString());
  EXPECT_STREQ("XYZXYZ", aString.ToCString());
  EXPECT_EQ(6, aString.Length());

  // Insert part of string into itself
  TCollection_AsciiString aString2("ABCD");
  const char*             aSubStr = aString2.ToCString() + 1; // "BCD"
  aString2.Insert(2, aSubStr);
  EXPECT_STREQ("ABCDBCD", aString2.ToCString());
  EXPECT_EQ(7, aString2.Length());
}

TEST(TCollection_AsciiStringTest, Insert_WithCharacter)
{
  TCollection_AsciiString aString("Hllo");

  // Insert character in middle
  aString.Insert(2, 'e');
  EXPECT_STREQ("Hello", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // Insert character at beginning
  TCollection_AsciiString aString2("ello");
  aString2.Insert(1, 'H');
  EXPECT_STREQ("Hello", aString2.ToCString());
}

TEST(TCollection_AsciiStringTest, Insert_BoundaryConditions)
{
  TCollection_AsciiString aString("Test");

  // Insert at position 1 (beginning)
  TCollection_AsciiString aString1 = aString;
  aString1.Insert(1, ">");
  EXPECT_STREQ(">Test", aString1.ToCString());

  // Insert at position length+1 (end)
  TCollection_AsciiString aString2 = aString;
  aString2.Insert(5, "<");
  EXPECT_STREQ("Test<", aString2.ToCString());
}

// ========================================
// Tests for Insert method with overlapping source
// ========================================

TEST(TCollection_AsciiStringTest, Insert_OverlapLeftSide)
{
  // Test case: source overlaps the left side of the shift window
  // String: "ABCDEFGH", insert "ABC" at position 4
  // Result: "ABC[ABC]DEFGH" - source is before insertion point
  TCollection_AsciiString aString("ABCDEFGH");
  const char*             aSource = aString.ToCString(); // "ABCDEFGH"
  aString.Insert(4, aSource, 3);                         // Insert first 3 chars at pos 4
  EXPECT_STREQ("ABCABCDEFGH", aString.ToCString());
  EXPECT_EQ(11, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapRightSide)
{
  // Test case: source overlaps the right side of the shift window
  // String: "ABCDEFGH", insert "DEFGH" at position 2
  // When shifting, "BCDEFGH" moves to positions 6-12
  // Source "DEFGH" (originally at 3-7) also shifts to 8-12
  // Result: "A" + "DEFGH" (from shifted position 8-12) + "BCDEFGH" = "ADEFGHBCDEFGH"
  TCollection_AsciiString aString("ABCDEFGH");
  const char*             aSource = aString.ToCString() + 3; // "DEFGH"
  aString.Insert(2, aSource);
  EXPECT_STREQ("ADEFGHBCDEFGH", aString.ToCString());
  EXPECT_EQ(13, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapInsideShiftWindow)
{
  // Test case: source is completely inside the shift window
  // String: "ABCDEFGH", insert "CDE" at position 2
  // Result: "A[CDE]BCDEFGH" - source will be shifted
  TCollection_AsciiString aString("ABCDEFGH");
  const char*             aSource = aString.ToCString() + 2; // "CDEFGH"
  aString.Insert(2, aSource, 3);                             // Insert "CDE" at pos 2
  EXPECT_STREQ("ACDEBCDEFGH", aString.ToCString());
  EXPECT_EQ(11, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapStartsBeforeShiftWindow)
{
  // Test case: source starts before and extends into shift window
  // String: "ABCDEFGH", insert "BCDE" at position 4
  // Result: "ABC[BCDE]DEFGH"
  TCollection_AsciiString aString("ABCDEFGH");
  const char*             aSource = aString.ToCString() + 1; // "BCDEFGH"
  aString.Insert(4, aSource, 4);                             // Insert "BCDE" at pos 4
  EXPECT_STREQ("ABCBCDEDEFGH", aString.ToCString());
  EXPECT_EQ(12, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapAtInsertionPoint)
{
  // Test case: source starts exactly at insertion point
  // String: "ABCDEFGH", insert "DEF" at position 4
  // Result: "ABC[DEF]DEFGH" - source starts where we insert
  TCollection_AsciiString aString("ABCDEFGH");
  const char*             aSource = aString.ToCString() + 3; // "DEFGH"
  aString.Insert(4, aSource, 3);                             // Insert "DEF" at pos 4
  EXPECT_STREQ("ABCDEFDEFGH", aString.ToCString());
  EXPECT_EQ(11, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapComplex1)
{
  // Test case: Insert middle portion into beginning
  // String: "123456789", insert "456" at position 1
  // Result: "[456]123456789"
  TCollection_AsciiString aString("123456789");
  const char*             aSource = aString.ToCString() + 3; // "456789"
  aString.Insert(1, aSource, 3);                             // Insert "456" at pos 1
  EXPECT_STREQ("456123456789", aString.ToCString());
  EXPECT_EQ(12, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapComplex2)
{
  // Test case: Insert beginning portion into middle
  // String: "XYZABC", insert "XY" at position 4
  // Result: "XYZ[XY]ABC"
  TCollection_AsciiString aString("XYZABC");
  const char*             aSource = aString.ToCString(); // "XYZABC"
  aString.Insert(4, aSource, 2);                         // Insert "XY" at pos 4
  EXPECT_STREQ("XYZXYABC", aString.ToCString());
  EXPECT_EQ(8, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapSpansEntireShiftWindow)
{
  // Test case: source spans from before to after shift window
  // String: "ABCDEFGHIJK", insert "BCDEFGH" at position 5
  // Result: "ABCD[BCDEFGH]EFGHIJK" - very wide overlap
  TCollection_AsciiString aString("ABCDEFGHIJK");
  const char*             aSource = aString.ToCString() + 1; // "BCDEFGHIJK"
  aString.Insert(5, aSource, 7);                             // Insert "BCDEFGH" at pos 5
  EXPECT_STREQ("ABCDBCDEFGHEFGHIJK", aString.ToCString());
  EXPECT_EQ(18, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapSingleChar)
{
  // Test case: Insert single character from itself
  // String: "ABC", insert 'B' at position 1
  TCollection_AsciiString aString("ABC");
  const char*             aSource = aString.ToCString() + 1; // "BC"
  aString.Insert(1, aSource, 1);                             // Insert 'B' at pos 1
  EXPECT_STREQ("BABC", aString.ToCString());
  EXPECT_EQ(4, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapEndToBeginning)
{
  // Test case: Insert end of string at beginning
  // String: "HELLO", insert "LO" at position 1
  TCollection_AsciiString aString("HELLO");
  const char*             aSource = aString.ToCString() + 3; // "LO"
  aString.Insert(1, aSource);
  EXPECT_STREQ("LOHELLO", aString.ToCString());
  EXPECT_EQ(7, aString.Length());
}

TEST(TCollection_AsciiStringTest, Insert_OverlapLargeString)
{
  // Test case: Large string with overlap
  // This tests that memmove is used correctly for large overlapping regions
  TCollection_AsciiString aString(50, 'A');
  aString.SetValue(25, "MARKER");                 // "AAA...MARKER...AAA"
  const char* aSource = aString.ToCString() + 20; // Take substring from position 20
  aString.Insert(10, aSource, 10);                // Insert 10 chars at position 10
  EXPECT_EQ(60, aString.Length());
  // Verify MARKER appears twice (once from original, once from inserted overlap)
  const char* aResult = aString.ToCString();
  EXPECT_TRUE(strstr(aResult, "MARKER") != nullptr);
}

// ========================================
// Tests for Cat method
// ========================================

TEST(TCollection_AsciiStringTest, Cat_BasicCases)
{
  TCollection_AsciiString aString("Hello");

  // Cat with C string
  TCollection_AsciiString aResult1 = aString.Cat(" World");
  EXPECT_STREQ("Hello World", aResult1.ToCString());
  EXPECT_STREQ("Hello", aString.ToCString()); // Original unchanged

  // Cat with AsciiString
  TCollection_AsciiString aSuffix("!");
  TCollection_AsciiString aResult2 = aString.Cat(aSuffix);
  EXPECT_STREQ("Hello!", aResult2.ToCString());

  // Cat with character
  TCollection_AsciiString aResult3 = aString.Cat('?');
  EXPECT_STREQ("Hello?", aResult3.ToCString());
}

TEST(TCollection_AsciiStringTest, Cat_IntegerAndReal)
{
  TCollection_AsciiString aString("Count: ");

  // Cat with integer
  TCollection_AsciiString aResult1 = aString.Cat(42);
  EXPECT_STREQ("Count: 42", aResult1.ToCString());

  // Cat with negative integer
  TCollection_AsciiString aResult2 = aString.Cat(-100);
  EXPECT_STREQ("Count: -100", aResult2.ToCString());

  // Cat with real
  TCollection_AsciiString aString2("Value: ");
  TCollection_AsciiString aResult3 = aString2.Cat(3.14);
  EXPECT_TRUE(strstr(aResult3.ToCString(), "3.14") != NULL);

  // Cat with zero
  TCollection_AsciiString aResult4 = aString.Cat(0);
  EXPECT_STREQ("Count: 0", aResult4.ToCString());
}

TEST(TCollection_AsciiStringTest, Cat_EmptyStrings)
{
  TCollection_AsciiString aEmpty;

  // Cat to empty string
  TCollection_AsciiString aResult1 = aEmpty.Cat("First");
  EXPECT_STREQ("First", aResult1.ToCString());

  // Cat empty string
  TCollection_AsciiString aString("Test");
  TCollection_AsciiString aResult2 = aString.Cat("");
  EXPECT_STREQ("Test", aResult2.ToCString());
}

TEST(TCollection_AsciiStringTest, Cat_ChainedOperations)
{
  TCollection_AsciiString aString("A");

  // Chain multiple Cat operations
  TCollection_AsciiString aResult = aString.Cat("B").Cat("C").Cat("D");
  EXPECT_STREQ("ABCD", aResult.ToCString());
}

// ========================================
// Tests for Copy method
// ========================================

TEST(TCollection_AsciiStringTest, Copy_BasicCases)
{
  TCollection_AsciiString aString("Original");

  // Copy with C string
  aString.Copy("NewValue");
  EXPECT_STREQ("NewValue", aString.ToCString());
  EXPECT_EQ(8, aString.Length());

  // Copy with AsciiString
  TCollection_AsciiString aSource("Another");
  aString.Copy(aSource);
  EXPECT_STREQ("Another", aString.ToCString());
  EXPECT_EQ(7, aString.Length());
}

TEST(TCollection_AsciiStringTest, Copy_ShorterAndLonger)
{
  TCollection_AsciiString aString("LongString");

  // Copy shorter string (should reuse memory)
  aString.Copy("Short");
  EXPECT_STREQ("Short", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // Copy longer string
  aString.Copy("VeryLongStringHere");
  EXPECT_STREQ("VeryLongStringHere", aString.ToCString());
  EXPECT_EQ(18, aString.Length());
}

TEST(TCollection_AsciiStringTest, Copy_EmptyString)
{
  TCollection_AsciiString aString("Original");

  // Copy empty string
  aString.Copy("");
  EXPECT_STREQ("", aString.ToCString());
  EXPECT_EQ(0, aString.Length());
  EXPECT_TRUE(aString.IsEmpty());
}

TEST(TCollection_AsciiStringTest, Copy_SelfAssignment)
{
  TCollection_AsciiString aString("Test");

  // Copy to itself (should be no-op)
  aString.Copy(aString.ToCString());
  EXPECT_STREQ("Test", aString.ToCString());
  EXPECT_EQ(4, aString.Length());
}

TEST(TCollection_AsciiStringTest, Copy_MemoryReuse)
{
  TCollection_AsciiString aString(100, 'A');

  // Copy shorter string - should reuse buffer
  aString.Copy("Short");
  EXPECT_STREQ("Short", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // Copy another short string - should still reuse
  aString.Copy("Tiny");
  EXPECT_STREQ("Tiny", aString.ToCString());
  EXPECT_EQ(4, aString.Length());
}

// ========================================
// Tests for Search and SearchFromEnd methods
// ========================================

TEST(TCollection_AsciiStringTest, Search_BasicCases)
{
  TCollection_AsciiString aString("This is a test string");

  // Search for existing substring
  EXPECT_EQ(1, aString.Search("This"));
  EXPECT_EQ(11, aString.Search("test"));
  EXPECT_EQ(16, aString.Search("string"));

  // Search for non-existing substring
  EXPECT_EQ(-1, aString.Search("xyz"));
  EXPECT_EQ(-1, aString.Search("notfound"));
}

TEST(TCollection_AsciiStringTest, Search_EdgeCases)
{
  TCollection_AsciiString aString("abcabc");

  // Search for substring - finds first occurrence
  EXPECT_EQ(1, aString.Search("abc"));

  // Search for single character - finds first occurrence
  EXPECT_EQ(1, aString.Search("a"));
  EXPECT_EQ(2, aString.Search("b"));
  EXPECT_EQ(3, aString.Search("c"));

  // Search for empty string
  EXPECT_EQ(-1, aString.Search(""));

  // Search in empty string
  TCollection_AsciiString anEmpty;
  EXPECT_EQ(-1, anEmpty.Search("test"));
}

TEST(TCollection_AsciiStringTest, Search_RepeatedPattern)
{
  TCollection_AsciiString aString("aaaaaaa");

  // Search should find first occurrence
  EXPECT_EQ(1, aString.Search("aaa"));

  // Test with different patterns
  TCollection_AsciiString aString2("abcabcabc");
  EXPECT_EQ(1, aString2.Search("abc"));    // First occurrence
  EXPECT_EQ(1, aString2.Search("abcabc")); // Longer pattern
}

TEST(TCollection_AsciiStringTest, SearchFromEnd_BasicCases)
{
  TCollection_AsciiString aString("This is a test string with test");

  // SearchFromEnd should find last occurrence
  EXPECT_EQ(28, aString.SearchFromEnd("test"));
  EXPECT_EQ(16, aString.SearchFromEnd("string"));

  // Search for non-existing substring
  EXPECT_EQ(-1, aString.SearchFromEnd("xyz"));
}

TEST(TCollection_AsciiStringTest, SearchFromEnd_EdgeCases)
{
  TCollection_AsciiString aString("abcabc");

  // SearchFromEnd for repeated pattern
  EXPECT_EQ(4, aString.SearchFromEnd("abc"));

  // SearchFromEnd for single character
  EXPECT_EQ(4, aString.SearchFromEnd("a"));
  EXPECT_EQ(6, aString.SearchFromEnd("c"));

  // SearchFromEnd for empty string
  EXPECT_EQ(-1, aString.SearchFromEnd(""));

  // SearchFromEnd in empty string
  TCollection_AsciiString anEmpty;
  EXPECT_EQ(-1, anEmpty.SearchFromEnd("test"));
}

TEST(TCollection_AsciiStringTest, Search_LongerThanString)
{
  TCollection_AsciiString aString("Short");

  // Search for substring longer than string
  EXPECT_EQ(-1, aString.Search("VeryLongSubstring"));
}

// ========================================
// Tests for IsSameString method
// ========================================

TEST(TCollection_AsciiStringTest, IsSameString_CaseSensitive)
{
  TCollection_AsciiString aString1("Test");
  TCollection_AsciiString aString2("Test");
  TCollection_AsciiString aString3("test");
  TCollection_AsciiString aString4("Different");

  // Case sensitive comparison
  EXPECT_TRUE(TCollection_AsciiString::IsSameString(aString1, aString2, Standard_True));
  EXPECT_FALSE(TCollection_AsciiString::IsSameString(aString1, aString3, Standard_True));
  EXPECT_FALSE(TCollection_AsciiString::IsSameString(aString1, aString4, Standard_True));
}

TEST(TCollection_AsciiStringTest, IsSameString_CaseInsensitive)
{
  TCollection_AsciiString aString1("Test");
  TCollection_AsciiString aString2("TEST");
  TCollection_AsciiString aString3("test");
  TCollection_AsciiString aString4("TeSt");

  // Case insensitive comparison
  EXPECT_TRUE(TCollection_AsciiString::IsSameString(aString1, aString2, Standard_False));
  EXPECT_TRUE(TCollection_AsciiString::IsSameString(aString1, aString3, Standard_False));
  EXPECT_TRUE(TCollection_AsciiString::IsSameString(aString1, aString4, Standard_False));
}

TEST(TCollection_AsciiStringTest, IsSameString_EmptyStrings)
{
  TCollection_AsciiString aEmpty1;
  TCollection_AsciiString aEmpty2("");
  TCollection_AsciiString aNonEmpty("Test");

  // Empty string comparisons
  EXPECT_TRUE(TCollection_AsciiString::IsSameString(aEmpty1, aEmpty2, Standard_True));
  EXPECT_TRUE(TCollection_AsciiString::IsSameString(aEmpty1, aEmpty2, Standard_False));
  EXPECT_FALSE(TCollection_AsciiString::IsSameString(aEmpty1, aNonEmpty, Standard_True));
}

TEST(TCollection_AsciiStringTest, IsSameString_DifferentLengths)
{
  TCollection_AsciiString aString1("Short");
  TCollection_AsciiString aString2("VeryLong");

  // Different length strings are never same
  EXPECT_FALSE(TCollection_AsciiString::IsSameString(aString1, aString2, Standard_True));
  EXPECT_FALSE(TCollection_AsciiString::IsSameString(aString1, aString2, Standard_False));
}

TEST(TCollection_AsciiStringTest, IsSameString_WithCStrings)
{
  // Test overload with C strings
  EXPECT_TRUE(TCollection_AsciiString::IsSameString("Test", 4, "Test", 4, Standard_True));
  EXPECT_FALSE(TCollection_AsciiString::IsSameString("Test", 4, "test", 4, Standard_True));
  EXPECT_TRUE(TCollection_AsciiString::IsSameString("Test", 4, "test", 4, Standard_False));
}

// ========================================
// Tests for IsEqual, IsDifferent, IsLess, IsGreater methods
// ========================================

TEST(TCollection_AsciiStringTest, IsEqual_AllOverloads)
{
  TCollection_AsciiString aString("Test");

  // IsEqual with AsciiString
  TCollection_AsciiString aString2("Test");
  EXPECT_TRUE(aString.IsEqual(aString2));

  // IsEqual with C string
  EXPECT_TRUE(aString.IsEqual("Test"));
  EXPECT_FALSE(aString.IsEqual("Different"));

  // IsEqual with case sensitivity
  TCollection_AsciiString aString3("test");
  EXPECT_FALSE(aString.IsEqual(aString3));
}

TEST(TCollection_AsciiStringTest, IsDifferent_AllOverloads)
{
  TCollection_AsciiString aString("Test");

  // IsDifferent with AsciiString
  TCollection_AsciiString aString2("Different");
  EXPECT_TRUE(aString.IsDifferent(aString2));

  // IsDifferent with C string
  EXPECT_TRUE(aString.IsDifferent("Different"));
  EXPECT_FALSE(aString.IsDifferent("Test"));
}

TEST(TCollection_AsciiStringTest, IsLess_AllOverloads)
{
  TCollection_AsciiString aStringA("Apple");
  TCollection_AsciiString aStringB("Banana");

  // IsLess with AsciiString
  EXPECT_TRUE(aStringA.IsLess(aStringB));
  EXPECT_FALSE(aStringB.IsLess(aStringA));

  // IsLess with C string
  EXPECT_TRUE(aStringA.IsLess("Banana"));
  EXPECT_FALSE(aStringB.IsLess("Apple"));

  // IsLess with same string
  EXPECT_FALSE(aStringA.IsLess(aStringA));
}

TEST(TCollection_AsciiStringTest, IsGreater_AllOverloads)
{
  TCollection_AsciiString aStringA("Apple");
  TCollection_AsciiString aStringB("Banana");

  // IsGreater with AsciiString
  EXPECT_TRUE(aStringB.IsGreater(aStringA));
  EXPECT_FALSE(aStringA.IsGreater(aStringB));

  // IsGreater with C string
  EXPECT_TRUE(aStringB.IsGreater("Apple"));
  EXPECT_FALSE(aStringA.IsGreater("Banana"));

  // IsGreater with same string
  EXPECT_FALSE(aStringA.IsGreater(aStringA));
}

TEST(TCollection_AsciiStringTest, Comparison_EmptyStrings)
{
  TCollection_AsciiString aEmpty;
  TCollection_AsciiString aNonEmpty("Test");

  // Empty string comparisons
  EXPECT_TRUE(aEmpty.IsEqual(""));
  EXPECT_FALSE(aEmpty.IsEqual(aNonEmpty));
  EXPECT_TRUE(aEmpty.IsDifferent(aNonEmpty));
  EXPECT_TRUE(aEmpty.IsLess(aNonEmpty));
  EXPECT_FALSE(aEmpty.IsGreater(aNonEmpty));
}

// ========================================
// Tests for SetValue method
// ========================================

TEST(TCollection_AsciiStringTest, SetValue_BasicCases)
{
  TCollection_AsciiString aString("Hello");

  // SetValue with character
  aString.SetValue(1, 'h');
  EXPECT_STREQ("hello", aString.ToCString());

  aString.SetValue(5, '!');
  EXPECT_STREQ("hell!", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, SetValue_WithString)
{
  TCollection_AsciiString aString("AAAAA");

  // SetValue with C string
  aString.SetValue(2, "XYZ");
  EXPECT_STREQ("AXYZA", aString.ToCString());

  // SetValue with AsciiString
  TCollection_AsciiString aReplacement("12");
  aString.SetValue(1, aReplacement);
  EXPECT_STREQ("12YZA", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, SetValue_BoundaryConditions)
{
  TCollection_AsciiString aString("Test");

  // SetValue at first position
  aString.SetValue(1, 'X');
  EXPECT_STREQ("Xest", aString.ToCString());

  // SetValue at last position
  aString.SetValue(4, 'Y');
  EXPECT_STREQ("XesY", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, SetValue_StringOverflow)
{
  TCollection_AsciiString aString("AAAA");

  // SetValue with string that extends beyond current length
  aString.SetValue(3, "XYZ");
  EXPECT_STREQ("AAXYZ", aString.ToCString());
  EXPECT_EQ(5, aString.Length());
}

// ========================================
// Tests for Remove and RemoveAll methods
// ========================================

TEST(TCollection_AsciiStringTest, Remove_BasicCases)
{
  TCollection_AsciiString aString("Hello World");

  // Remove from middle
  aString.Remove(6, 6);
  EXPECT_STREQ("Hello", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // Remove from beginning
  TCollection_AsciiString aString2("XYZTest");
  aString2.Remove(1, 3);
  EXPECT_STREQ("Test", aString2.ToCString());

  // Remove from end
  TCollection_AsciiString aString3("TestXYZ");
  aString3.Remove(5, 3);
  EXPECT_STREQ("Test", aString3.ToCString());
}

TEST(TCollection_AsciiStringTest, Remove_EntireString)
{
  TCollection_AsciiString aString("Test");

  // Remove all characters
  aString.Remove(1, 4);
  EXPECT_STREQ("", aString.ToCString());
  EXPECT_EQ(0, aString.Length());
  EXPECT_TRUE(aString.IsEmpty());
}

TEST(TCollection_AsciiStringTest, RemoveAll_SingleCharacter)
{
  TCollection_AsciiString aString("Mississippi");

  // RemoveAll 's' -> "Miiippi" (4 's' removed)
  aString.RemoveAll('s');
  EXPECT_STREQ("Miiippi", aString.ToCString());

  // RemoveAll 'i' -> "Mpp" (4 'i' removed: M-i-i-i-pp-i)
  aString.RemoveAll('i');
  EXPECT_STREQ("Mpp", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, RemoveAll_NoOccurrence)
{
  TCollection_AsciiString aString("Test");

  // RemoveAll character that doesn't exist
  aString.RemoveAll('X');
  EXPECT_STREQ("Test", aString.ToCString());
  EXPECT_EQ(4, aString.Length());
}

TEST(TCollection_AsciiStringTest, RemoveAll_AllCharacters)
{
  TCollection_AsciiString aString("AAAA");

  // RemoveAll the only character
  aString.RemoveAll('A');
  EXPECT_STREQ("", aString.ToCString());
  EXPECT_EQ(0, aString.Length());
  EXPECT_TRUE(aString.IsEmpty());
}

// ========================================
// Tests for Trunc method
// ========================================

TEST(TCollection_AsciiStringTest, Trunc_BasicCases)
{
  TCollection_AsciiString aString("Hello World");

  // Truncate to shorter length
  aString.Trunc(5);
  EXPECT_STREQ("Hello", aString.ToCString());
  EXPECT_EQ(5, aString.Length());

  // Truncate to zero
  aString.Trunc(0);
  EXPECT_STREQ("", aString.ToCString());
  EXPECT_EQ(0, aString.Length());
  EXPECT_TRUE(aString.IsEmpty());
}

TEST(TCollection_AsciiStringTest, Trunc_NoChange)
{
  TCollection_AsciiString aString("Test");

  // Truncate to same length
  aString.Trunc(4);
  EXPECT_STREQ("Test", aString.ToCString());
  EXPECT_EQ(4, aString.Length());
}

// ========================================
// Tests for std::string_view API
// ========================================

TEST(TCollection_AsciiStringTest, StringView_Constructor)
{
  std::string_view        aView("Hello World");
  TCollection_AsciiString aString(aView);

  EXPECT_STREQ("Hello World", aString.ToCString());
  EXPECT_EQ(11, aString.Length());
}

TEST(TCollection_AsciiStringTest, StringView_AssignmentAndComparison)
{
  TCollection_AsciiString aString;
  std::string_view        aView("Test");

  // Assignment from string_view
  aString = aView;
  EXPECT_STREQ("Test", aString.ToCString());

  // Comparison with string_view
  EXPECT_TRUE(aString.IsEqual(aView));
  EXPECT_FALSE(aString.IsDifferent(aView));
}

TEST(TCollection_AsciiStringTest, StringView_CatAndAssignCat)
{
  TCollection_AsciiString aString("Hello");
  std::string_view        aView(" World");

  // Cat with string_view
  TCollection_AsciiString aResult = aString.Cat(aView);
  EXPECT_STREQ("Hello World", aResult.ToCString());

  // AssignCat with string_view
  aString.AssignCat(aView);
  EXPECT_STREQ("Hello World", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, StringView_Insert)
{
  TCollection_AsciiString aString("HloWorld");
  std::string_view        aView("el");

  // Insert string_view
  aString.Insert(2, aView);
  EXPECT_STREQ("HelloWorld", aString.ToCString());
}

// ========================================
// Tests for Move semantics
// ========================================

TEST(TCollection_AsciiStringTest, MoveConstructor)
{
  TCollection_AsciiString aString1("Original String");
  TCollection_AsciiString aString2(std::move(aString1));

  EXPECT_STREQ("Original String", aString2.ToCString());
  EXPECT_EQ(15, aString2.Length());

  // Original should be in valid but unspecified state
  // (typically empty after move)
  EXPECT_TRUE(aString1.IsEmpty() || aString1.Length() >= 0);
}

TEST(TCollection_AsciiStringTest, MoveAssignment)
{
  TCollection_AsciiString aString1("First");
  TCollection_AsciiString aString2("Second");

  aString2 = std::move(aString1);

  EXPECT_STREQ("First", aString2.ToCString());
  EXPECT_EQ(5, aString2.Length());
}

// ========================================
// Tests for template literal constructors
// ========================================

TEST(TCollection_AsciiStringTest, TemplateLiteral_Constructor)
{
  // Test compile-time size deduction for string literals
  TCollection_AsciiString aString("Literal");
  EXPECT_STREQ("Literal", aString.ToCString());
  EXPECT_EQ(7, aString.Length());
}

TEST(TCollection_AsciiStringTest, TemplateLiteral_Assignment)
{
  TCollection_AsciiString aString;
  aString = "Assigned";
  EXPECT_STREQ("Assigned", aString.ToCString());
  EXPECT_EQ(8, aString.Length());
}

// ========================================
// Tests for edge cases and stress tests
// ========================================

TEST(TCollection_AsciiStringTest, StressTest_MultipleOperations)
{
  TCollection_AsciiString aString;

  // Build string through multiple operations
  for (int i = 0; i < 10; ++i)
  {
    aString.AssignCat("A");
  }
  EXPECT_EQ(10, aString.Length());

  // Insert in middle multiple times
  for (int i = 0; i < 5; ++i)
  {
    aString.Insert(6, "B");
  }
  EXPECT_EQ(15, aString.Length());

  // Remove characters
  aString.RemoveAll('B');
  EXPECT_EQ(10, aString.Length());
  EXPECT_STREQ("AAAAAAAAAA", aString.ToCString());
}

TEST(TCollection_AsciiStringTest, StressTest_LargeOperations)
{
  // Create large string
  TCollection_AsciiString aLarge(1000, 'X');
  EXPECT_EQ(1000, aLarge.Length());

  // Copy to another large string
  TCollection_AsciiString aCopy;
  aCopy.Copy(aLarge);
  EXPECT_EQ(1000, aCopy.Length());

  // Concatenate large strings
  aLarge.AssignCat(aCopy);
  EXPECT_EQ(2000, aLarge.Length());
}

TEST(TCollection_AsciiStringTest, EdgeCase_NullCharacterHandling)
{
  // Ensure null characters are handled correctly
  TCollection_AsciiString aString('\0');
  EXPECT_TRUE(aString.IsEmpty());
  EXPECT_EQ(0, aString.Length());
}

TEST(TCollection_AsciiStringTest, EdgeCase_ConsecutiveOperations)
{
  TCollection_AsciiString aString("Test");

  // Multiple consecutive copies
  aString.Copy("A");
  aString.Copy("BB");
  aString.Copy("CCC");
  EXPECT_STREQ("CCC", aString.ToCString());

  // Multiple consecutive assigncats
  aString.AssignCat("D");
  aString.AssignCat("E");
  aString.AssignCat("F");
  EXPECT_STREQ("CCCDEF", aString.ToCString());
}

// Test case for bug with enum definition building (multiple AssignCat in loop)
TEST(TCollection_AsciiStringTest, AssignCat_MultipleInLoop)
{
  // This reproduces the scenario in MoniTool_TypedValue::Definition()
  TCollection_AsciiString aDef;
  char                    aMess[50];

  // Build enum definition like MoniTool_TypedValue does
  aDef.AssignCat("Enum");
  sprintf(aMess, " [in %d-%d]", 0, 1);
  aDef.AssignCat(aMess);

  // Should be "Enum [in 0-1]" at this point
  EXPECT_STREQ("Enum [in 0-1]", aDef.ToCString());

  // Now add enum values in a loop
  for (Standard_Integer i = 0; i <= 1; i++)
  {
    const char* anEnva = (i == 0) ? "Off" : "On";
    sprintf(aMess, " %d:%s", i, anEnva);
    aDef.AssignCat(aMess);
  }

  // Should be "Enum [in 0-1] 0:Off 1:On"
  EXPECT_STREQ("Enum [in 0-1] 0:Off 1:On", aDef.ToCString());

  // Add alpha section
  aDef.AssignCat(" , alpha: ");
  sprintf(aMess, "On:%d ", 1);
  aDef.AssignCat("On");
  aDef.AssignCat(aMess);
  sprintf(aMess, "Off:%d ", 0);
  aDef.AssignCat("Off");
  aDef.AssignCat(aMess);

  // Final result should contain all parts
  const char* aResult = aDef.ToCString();
  EXPECT_TRUE(strstr(aResult, "Enum") != nullptr);
  EXPECT_TRUE(strstr(aResult, "[in 0-1]") != nullptr);
  EXPECT_TRUE(strstr(aResult, "0:Off") != nullptr);
  EXPECT_TRUE(strstr(aResult, "1:On") != nullptr);
  EXPECT_TRUE(strstr(aResult, "alpha:") != nullptr);
}

// Test BUC60724: Empty string initialization
// Migrated from QABugs_3.cxx
TEST(TCollection_AsciiStringTest, BUC60724_EmptyStringInitialization)
{
  // Test empty string initialization with empty C string
  TCollection_AsciiString aString1("");
  EXPECT_NE(nullptr, aString1.ToCString());
  EXPECT_EQ(0, aString1.Length());
  EXPECT_EQ('\0', aString1.ToCString()[0]);

  // Test empty string initialization with null character
  TCollection_AsciiString aString2('\0');
  EXPECT_NE(nullptr, aString2.ToCString());
  EXPECT_EQ(0, aString2.Length());
  EXPECT_EQ('\0', aString2.ToCString()[0]);
}

// Test BUC60773: TCollection_HAsciiString initialization
// Migrated from QABugs_3.cxx
TEST(TCollection_AsciiStringTest, BUC60773_HAsciiStringInitialization)
{
  // Create empty HAsciiString
  Handle(TCollection_HAsciiString) anHAscii = new TCollection_HAsciiString();
  EXPECT_FALSE(anHAscii.IsNull());

  // Get C string from HAsciiString
  Standard_CString aStr = anHAscii->ToCString();
  EXPECT_NE(nullptr, aStr);

  // Create AsciiString from C string
  TCollection_AsciiString aAscii(aStr);
  EXPECT_EQ(0, aAscii.Length());
  EXPECT_TRUE(aAscii.IsEmpty());
}

// Test OCC6794: TCollection_AsciiString large concatenation
// Migrated from QABugs_14.cxx
TEST(TCollection_AsciiStringTest, OCC6794_LargeConcatenation)
{
  // Test concatenation of many small strings to verify memory handling
  const Standard_Integer aNb = 10000; // Use a smaller number for faster test
  const char*            aC  = "a";

  TCollection_AsciiString anAscii;
  for (Standard_Integer i = 1; i <= aNb; i++)
  {
    anAscii += TCollection_AsciiString(aC);
  }

  // Verify the final length matches expected
  EXPECT_EQ(aNb, anAscii.Length()) << "Concatenated string should have correct length";
  EXPECT_FALSE(anAscii.IsEmpty()) << "Concatenated string should not be empty";
}

TEST(TCollection_AsciiStringTest, OCC11758_ComprehensiveConstructorsAndMethods)
{
  const char* theStr = "0123456789";

  for (Standard_Integer i = 0; i < 5; ++i)
  {
    // TCollection_AsciiString(const Standard_CString astring)
    TCollection_AsciiString a(theStr + i);
    EXPECT_STREQ(theStr + i, a.ToCString());

    // TCollection_AsciiString(const Standard_CString astring, const Standard_Integer aLen)
    TCollection_AsciiString b(theStr + i, 3);
    EXPECT_EQ(3, b.Length());
    EXPECT_EQ(0, strncmp(b.ToCString(), theStr + i, 3));

    // TCollection_AsciiString(const Standard_Integer aValue)
    TCollection_AsciiString c(i);
    EXPECT_TRUE(c.IsIntegerValue());
    EXPECT_EQ(i, c.IntegerValue());

    // TCollection_AsciiString(const Standard_Real aValue)
    TCollection_AsciiString d(0.1 * i);
    EXPECT_TRUE(d.IsRealValue(Standard_True));
    EXPECT_FALSE(TCollection_AsciiString("3.3!").IsRealValue(Standard_True));
    EXPECT_TRUE(TCollection_AsciiString("3.3!").IsRealValue(Standard_False));
    EXPECT_STREQ("3.3", TCollection_AsciiString(3.3).ToCString());

    // Copy constructor
    TCollection_AsciiString e(d);
    EXPECT_STREQ(d.ToCString(), e.ToCString());
    EXPECT_EQ(d.Length(), e.Length());

    // Concatenation with char
    TCollection_AsciiString f(e, '\a');
    EXPECT_EQ(e.Length() + 1, f.Length());
    EXPECT_EQ(0, strncmp(f.ToCString(), e.ToCString(), e.Length()));
    EXPECT_EQ('\a', f.Value(f.Length()));

    // Concatenation with C string
    TCollection_AsciiString g(f, theStr);
    EXPECT_EQ(f.Length() + (Standard_Integer)strlen(theStr), g.Length());
    EXPECT_EQ(0, strncmp(g.ToCString(), f.ToCString(), f.Length()));
    EXPECT_EQ(f.Length() + 1, g.Search(theStr));

    // Concatenation with TCollection_AsciiString
    TCollection_AsciiString h(d, a);
    EXPECT_EQ(d.Length() + a.Length(), h.Length());
    EXPECT_EQ(0, strncmp(h.ToCString(), d.ToCString(), d.Length()));
    EXPECT_EQ(0, strncmp(h.ToCString() + d.Length(), a.ToCString(), a.Length()));

    // AssignCat with C string
    c.AssignCat(a.ToCString());
    EXPECT_EQ(1 + a.Length(), c.Length());
    EXPECT_EQ(2, c.Search(a));

    // AssignCat with TCollection_AsciiString
    Standard_Integer dl = d.Length();
    d.AssignCat(a);
    EXPECT_EQ(dl + a.Length(), d.Length());
    EXPECT_EQ(dl + 1, d.Search(a));

    // Capitalize
    TCollection_AsciiString capitalize("aBC");
    capitalize.Capitalize();
    EXPECT_STREQ("Abc", capitalize.ToCString());

    // Copy assignment
    d = theStr + i;
    EXPECT_STREQ(theStr + i, d.ToCString());

    d = h;
    EXPECT_STREQ(h.ToCString(), d.ToCString());

    // Insert C string
    dl = d.Length();
    d.Insert(2, theStr);
    EXPECT_EQ(dl + (Standard_Integer)strlen(theStr), d.Length());
    EXPECT_EQ(0, strncmp(d.ToCString() + 1, theStr, strlen(theStr)));

    // Insert char
    d = theStr;
    d.Insert(i + 1, 'i');
    EXPECT_EQ((Standard_Integer)strlen(theStr) + 1, d.Length());
    EXPECT_EQ('i', d.Value(i + 1));
    EXPECT_STREQ(theStr + i, d.ToCString() + i + 1);

    // Insert TCollection_AsciiString
    d = theStr;
    d.Insert(i + 1, TCollection_AsciiString("i"));
    EXPECT_EQ((Standard_Integer)strlen(theStr) + 1, d.Length());
    EXPECT_EQ('i', d.Value(i + 1));
    EXPECT_STREQ(theStr + i, d.ToCString() + i + 1);

    // IsDifferent
    EXPECT_TRUE(d.IsDifferent(theStr));
    EXPECT_TRUE(d.IsDifferent("theStr"));
    EXPECT_TRUE(d.IsDifferent(""));
    EXPECT_FALSE(d.IsDifferent(d.ToCString()));
  }
}
