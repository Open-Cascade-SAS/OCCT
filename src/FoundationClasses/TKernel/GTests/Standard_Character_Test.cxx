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

#include <Standard_Character.hxx>

#include <gtest/gtest.h>

TEST(Standard_Character_Test, OCC29925_CharacterClassificationFunctions)
{
  // Test that all character classification functions work without crashes
  // for all valid ASCII chars (including extended)
  for (int i = 0; i < 256; i++)
  {
    Standard_Character c = (char)(unsigned char)i;

    // These functions should not throw exceptions for any valid ASCII character
    EXPECT_NO_THROW(IsAlphabetic(c));
    EXPECT_NO_THROW(IsDigit(c));
    EXPECT_NO_THROW(IsXDigit(c));
    EXPECT_NO_THROW(IsAlphanumeric(c));
    EXPECT_NO_THROW(IsControl(c));
    EXPECT_NO_THROW(IsGraphic(c));
    EXPECT_NO_THROW(IsLowerCase(c));
    EXPECT_NO_THROW(IsPrintable(c));
    EXPECT_NO_THROW(IsPunctuation(c));
    EXPECT_NO_THROW(IsSpace(c));
    EXPECT_NO_THROW(IsUpperCase(c));
    EXPECT_NO_THROW(LowerCase(c));
    EXPECT_NO_THROW(UpperCase(c));
  }

  // Verify some specific character classifications
  EXPECT_TRUE(IsAlphabetic('A'));
  EXPECT_TRUE(IsAlphabetic('z'));
  EXPECT_FALSE(IsAlphabetic('5'));

  EXPECT_TRUE(IsDigit('0'));
  EXPECT_TRUE(IsDigit('9'));
  EXPECT_FALSE(IsDigit('A'));

  EXPECT_TRUE(IsSpace(' '));
  EXPECT_TRUE(IsSpace('\t'));
  EXPECT_FALSE(IsSpace('A'));

  EXPECT_TRUE(IsLowerCase('a'));
  EXPECT_FALSE(IsLowerCase('A'));

  EXPECT_TRUE(IsUpperCase('A'));
  EXPECT_FALSE(IsUpperCase('a'));

  EXPECT_EQ('a', LowerCase('A'));
  EXPECT_EQ('A', UpperCase('a'));
}
