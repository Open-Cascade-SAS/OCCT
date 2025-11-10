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
  const Standard_ExtCharacter unicodeChars[] = {0x0041,
                                                0x00A9,
                                                0x2122,
                                                0x20AC,
                                                0}; // A, copyright, trademark, euro
  TCollection_ExtendedString  unicodeString(unicodeChars);

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

TEST(TCollection_ExtendedStringTest, UTF8Conversion)
{
  // Test the LengthOfCString() and ToUTF8CString() combination
  TCollection_ExtendedString aString("Hello World");

  Standard_Integer aBufferSize = aString.LengthOfCString();
  EXPECT_GT(aBufferSize, 0);

  // Allocate buffer with +1 for null terminator (external usage pattern)
  Standard_PCharacter aBuffer        = new Standard_Character[aBufferSize + 1];
  Standard_Integer    anActualLength = aString.ToUTF8CString(aBuffer);

  EXPECT_EQ(aBufferSize, anActualLength);
  EXPECT_EQ('\0', aBuffer[anActualLength]);
  EXPECT_STREQ("Hello World", aBuffer);

  delete[] aBuffer;
}

TEST(TCollection_ExtendedStringTest, UTF8ConversionUnicode)
{
  // Test UTF-8 conversion with Unicode characters
  const Standard_ExtCharacter aUnicodeStr[] =
    {0x0048, 0x00E9, 0x006C, 0x006C, 0x006F, 0}; // "H(e-acute)llo"
  TCollection_ExtendedString aString(aUnicodeStr);

  Standard_Integer aBufferSize = aString.LengthOfCString();
  EXPECT_GT(aBufferSize, 5); // Should be more than 5 due to UTF-8 encoding

  Standard_PCharacter aBuffer        = new Standard_Character[aBufferSize + 1];
  Standard_Integer    anActualLength = aString.ToUTF8CString(aBuffer);

  EXPECT_EQ(aBufferSize, anActualLength);
  EXPECT_EQ('\0', aBuffer[anActualLength]);

  delete[] aBuffer;
}

TEST(TCollection_ExtendedStringTest, WideCharConstructor)
{
  // Test constructor with wide characters
  const Standard_WideChar*   aWideStr = L"Wide string test";
  TCollection_ExtendedString aString(aWideStr);

  EXPECT_GT(aString.Length(), 0);
  EXPECT_FALSE(aString.IsEmpty());
}

TEST(TCollection_ExtendedStringTest, NumericalConstructors)
{
  // Test integer constructor
  TCollection_ExtendedString anIntString(42);
  TCollection_AsciiString    anAsciiFromInt(anIntString);
  EXPECT_STREQ("42", anAsciiFromInt.ToCString());

  // Test real constructor
  TCollection_ExtendedString aRealString(3.14);
  TCollection_AsciiString    anAsciiFromReal(aRealString);
  const Standard_CString     aRealCStr = anAsciiFromReal.ToCString();
  EXPECT_TRUE(strstr(aRealCStr, "3.14") != NULL);
}

TEST(TCollection_ExtendedStringTest, FillerConstructor)
{
  // Test constructor with length and filler character
  TCollection_ExtendedString aFilledString(5, 'X');
  EXPECT_EQ(5, aFilledString.Length());

  TCollection_AsciiString anAsciiFromFilled(aFilledString);
  EXPECT_STREQ("XXXXX", anAsciiFromFilled.ToCString());
}

TEST(TCollection_ExtendedStringTest, ExtendedCharConstructor)
{
  // Test constructor with ExtendedCharacter
  const Standard_ExtCharacter aEuroChar = 0x20AC; // Euro symbol
  TCollection_ExtendedString  aString(aEuroChar);

  EXPECT_EQ(1, aString.Length());
  EXPECT_FALSE(aString.IsAscii());
  EXPECT_EQ(aEuroChar, aString.Value(1));
}

TEST(TCollection_ExtendedStringTest, UnicodeCharacters)
{
  // Test various Unicode characters
  const Standard_ExtCharacter aLatinA = 0x0041; // 'A'
  const Standard_ExtCharacter aLatinE = 0x00E9; // 'e-acute'
  const Standard_ExtCharacter aEuro   = 0x20AC; // Euro symbol
  const Standard_ExtCharacter aCJK    = 0x4E2D; // Chinese character

  const Standard_ExtCharacter aUnicodeStr[] = {aLatinA, aLatinE, aEuro, aCJK, 0};
  TCollection_ExtendedString  aString(aUnicodeStr);

  EXPECT_EQ(4, aString.Length());
  EXPECT_EQ(aLatinA, aString.Value(1));
  EXPECT_EQ(aLatinE, aString.Value(2));
  EXPECT_EQ(aEuro, aString.Value(3));
  EXPECT_EQ(aCJK, aString.Value(4));
  EXPECT_FALSE(aString.IsAscii());
}

TEST(TCollection_ExtendedStringTest, AsciiDetection)
{
  // Test ASCII detection
  TCollection_ExtendedString anAsciiString("Simple ASCII");
  EXPECT_TRUE(anAsciiString.IsAscii());

  const Standard_ExtCharacter aNonAsciiStr[] = {0x0041, 0x20AC, 0}; // A + Euro
  TCollection_ExtendedString  aNonAsciiString(aNonAsciiStr);
  EXPECT_FALSE(aNonAsciiString.IsAscii());
}

TEST(TCollection_ExtendedStringTest, EmptyStringHandling)
{
  // Test empty string operations
  TCollection_ExtendedString anEmptyString;
  EXPECT_EQ(0, anEmptyString.Length());
  EXPECT_TRUE(anEmptyString.IsEmpty());
  EXPECT_EQ(0, anEmptyString.LengthOfCString());

  Standard_PCharacter aBuffer = new Standard_Character[1];
  Standard_Integer    aLength = anEmptyString.ToUTF8CString(aBuffer);
  EXPECT_EQ(0, aLength);
  EXPECT_EQ('\0', aBuffer[0]);

  delete[] aBuffer;
}

TEST(TCollection_ExtendedStringTest, ConversionRoundTrip)
{
  // Test AsciiString <-> ExtendedString conversion
  const Standard_CString anOriginalStr = "Test conversion with special chars: !@#$%";

  TCollection_AsciiString    anAsciiOriginal(anOriginalStr);
  TCollection_ExtendedString anExtendedConverted(anAsciiOriginal);
  TCollection_AsciiString    anAsciiRoundTrip(anExtendedConverted);

  EXPECT_STREQ(anOriginalStr, anAsciiRoundTrip.ToCString());
  EXPECT_EQ(anAsciiOriginal.Length(), anExtendedConverted.Length());
  EXPECT_EQ(anAsciiOriginal.Length(), anAsciiRoundTrip.Length());
}

TEST(TCollection_ExtendedStringTest, LargeStrings)
{
  // Test with large strings
  const Standard_Integer     aLargeSize = 1000;
  TCollection_ExtendedString aLargeString(aLargeSize, 'A');

  EXPECT_EQ(aLargeSize, aLargeString.Length());
  EXPECT_EQ('A', aLargeString.Value(1));
  EXPECT_EQ('A', aLargeString.Value(aLargeSize));
  EXPECT_TRUE(aLargeString.IsAscii());
}

TEST(TCollection_ExtendedStringTest, MemoryAllocation)
{
  // Test memory allocation with various string lengths
  for (Standard_Integer anIdx = 1; anIdx <= 16; ++anIdx)
  {
    TCollection_ExtendedString aTestString(anIdx, 'X');
    EXPECT_EQ(anIdx, aTestString.Length());
    EXPECT_EQ('X', aTestString.Value(1));

    if (anIdx > 1)
    {
      EXPECT_EQ('X', aTestString.Value(anIdx));
    }
  }
}

TEST(TCollection_ExtendedStringTest, MultiByteCString)
{
  // Test constructor with multibyte flag
  const Standard_CString     aMultiByteStr = "Multi-byte test";
  TCollection_ExtendedString aString(aMultiByteStr, Standard_True);

  EXPECT_GT(aString.Length(), 0);
  EXPECT_FALSE(aString.IsEmpty());
}

TEST(TCollection_ExtendedStringTest, BoundaryValues)
{
  // Test boundary Unicode values
  // Note: OCCT's IsAnAscii considers 0x00-0xFF as ASCII (full 8-bit range)
  const Standard_ExtCharacter aLastStandardAscii = 0x007F;
  const Standard_ExtCharacter aLastOCCTAscii     = 0x00FF;
  const Standard_ExtCharacter aFirstExtended     = 0x0100;
  const Standard_ExtCharacter aMaxBMP            = 0xFFFF;

  // Test individual characters
  TCollection_ExtendedString aStringLastStandardAscii(aLastStandardAscii);
  EXPECT_EQ(1, aStringLastStandardAscii.Length());
  EXPECT_TRUE(aStringLastStandardAscii.IsAscii());

  TCollection_ExtendedString aStringLastOCCTAscii(aLastOCCTAscii);
  EXPECT_EQ(1, aStringLastOCCTAscii.Length());
  EXPECT_TRUE(aStringLastOCCTAscii.IsAscii());

  TCollection_ExtendedString aStringFirstExtended(aFirstExtended);
  EXPECT_EQ(1, aStringFirstExtended.Length());
  EXPECT_FALSE(aStringFirstExtended.IsAscii());

  TCollection_ExtendedString aStringMaxBMP(aMaxBMP);
  EXPECT_EQ(1, aStringMaxBMP.Length());
  EXPECT_FALSE(aStringMaxBMP.IsAscii());
}

// Test TestMem: Large string memory allocation
// Migrated from QABugs_3.cxx
TEST(TCollection_ExtendedStringTest, TestMem_LargeStringAllocation)
{
  // Test allocation of a large extended string (1MB of characters)
  // This test verifies that the string can handle large allocations without crashing
  const Standard_Integer     aLargeSize = 1024 * 1024;
  TCollection_ExtendedString aString(aLargeSize, 'A');

  EXPECT_EQ(aLargeSize, aString.Length());
  EXPECT_FALSE(aString.IsEmpty());
}

// Test OCC3277: TCollection_ExtendedString Cat operation
TEST(TCollection_ExtendedStringTest, OCC3277_CatOperation)
{
  // Test concatenation of an input string to an empty extended string
  TCollection_ExtendedString anExtendedString;
  TCollection_ExtendedString anInputString("TestString");

  // Cat() returns a new string, it doesn't modify the original
  TCollection_ExtendedString aResult = anExtendedString.Cat(anInputString);

  // Verify the result
  EXPECT_EQ(anInputString.Length(), aResult.Length())
    << "Concatenated string should have same length as input";
  EXPECT_FALSE(aResult.IsEmpty()) << "Concatenated string should not be empty";

  // Verify the content matches
  EXPECT_EQ(anInputString, aResult) << "Concatenated string should match input string";
}
