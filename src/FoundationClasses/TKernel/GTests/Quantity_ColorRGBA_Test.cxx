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

#include <Quantity_ColorRGBA.hxx>
#include <Quantity_Color.hxx>

#include <gtest/gtest.h>
#include <cmath>

// Test fixture for Quantity_ColorRGBA tests
class Quantity_ColorRGBATest : public testing::Test
{
protected:
  void SetUp() override {}

  void TearDown() override {}

  // Helper to compare floating point values
  bool IsNear(Standard_Real theValue1,
              Standard_Real theValue2,
              Standard_Real theTolerance = 0.001) const
  {
    return std::abs(theValue1 - theValue2) < theTolerance;
  }
};

// Test basic construction
TEST_F(Quantity_ColorRGBATest, BasicConstruction)
{
  // Default constructor
  Quantity_ColorRGBA aColor1;
  EXPECT_TRUE(IsNear(1.0f, aColor1.GetRGB().Red()));   // YELLOW = RGB(1,1,0)
  EXPECT_TRUE(IsNear(1.0f, aColor1.GetRGB().Green())); // YELLOW = RGB(1,1,0)
  EXPECT_TRUE(IsNear(0.0f, aColor1.GetRGB().Blue()));  // YELLOW = RGB(1,1,0)
  EXPECT_TRUE(IsNear(1.0f, aColor1.Alpha()));

  // Constructor with RGB + alpha
  Quantity_ColorRGBA aColor2(Quantity_Color(0.5, 0.6, 0.7, Quantity_TOC_RGB), 0.8f);
  EXPECT_TRUE(IsNear(0.5f, aColor2.GetRGB().Red()));
  EXPECT_TRUE(IsNear(0.6f, aColor2.GetRGB().Green()));
  EXPECT_TRUE(IsNear(0.7f, aColor2.GetRGB().Blue()));
  EXPECT_TRUE(IsNear(0.8f, aColor2.Alpha()));

  // Constructor with RGBA floats
  Quantity_ColorRGBA aColor3(0.3f, 0.4f, 0.5f, 0.6f);
  EXPECT_TRUE(IsNear(0.3f, aColor3.GetRGB().Red()));
  EXPECT_TRUE(IsNear(0.4f, aColor3.GetRGB().Green()));
  EXPECT_TRUE(IsNear(0.5f, aColor3.GetRGB().Blue()));
  EXPECT_TRUE(IsNear(0.6f, aColor3.Alpha()));
}

// Test constexpr capabilities
TEST_F(Quantity_ColorRGBATest, ConstexprGetters)
{
  const Quantity_ColorRGBA aColor(0.2f, 0.4f, 0.6f, 0.8f);

  // These should work with constexpr
  Standard_ShortReal aAlpha = aColor.Alpha();
  EXPECT_TRUE(IsNear(0.8f, aAlpha));
}

// Test SetValues
TEST_F(Quantity_ColorRGBATest, SetValues)
{
  Quantity_ColorRGBA aColor;

  aColor.SetValues(0.1f, 0.2f, 0.3f, 0.4f);
  EXPECT_TRUE(IsNear(0.1f, aColor.GetRGB().Red()));
  EXPECT_TRUE(IsNear(0.2f, aColor.GetRGB().Green()));
  EXPECT_TRUE(IsNear(0.3f, aColor.GetRGB().Blue()));
  EXPECT_TRUE(IsNear(0.4f, aColor.Alpha()));
}

// Test hex color parsing (tests new HEX_BASE constant)
TEST_F(Quantity_ColorRGBATest, HexColorParsing_RGB)
{
  Quantity_ColorRGBA aColor;

  // Test standard RGB hex format: #RRGGBB
  bool aResult = Quantity_ColorRGBA::ColorFromHex("#FF0000", aColor, true);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Red(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Green(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Blue(), 0.01f));

  // Test without # prefix
  aResult = Quantity_ColorRGBA::ColorFromHex("00FF00", aColor, true);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Red(), 0.01f));
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Green(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Blue(), 0.01f));

  // Test blue
  aResult = Quantity_ColorRGBA::ColorFromHex("#0000FF", aColor, true);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Red(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Green(), 0.01f));
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Blue(), 0.01f));
}

// Test RGBA hex format
TEST_F(Quantity_ColorRGBATest, HexColorParsing_RGBA)
{
  Quantity_ColorRGBA aColor;

  // Test RGBA hex format: #RRGGBBAA
  bool aResult = Quantity_ColorRGBA::ColorFromHex("#FF000080", aColor, false);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Red(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Green(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Blue(), 0.01f));
  EXPECT_TRUE(IsNear(0.5f, aColor.Alpha(), 0.02f)); // 0x80 / 0xFF ~= 0.5
}

// Test short hex format (tests HEX_BITS_PER_COMPONENT_SHORT constant)
TEST_F(Quantity_ColorRGBATest, HexColorParsing_ShortRGB)
{
  Quantity_ColorRGBA aColor;

  // Test short RGB hex format: #RGB (each component is 0-F)
  bool aResult = Quantity_ColorRGBA::ColorFromHex("#F00", aColor, true);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Red(), 0.1f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Green(), 0.1f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Blue(), 0.1f));

  // Test short format for white
  aResult = Quantity_ColorRGBA::ColorFromHex("#FFF", aColor, true);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Red(), 0.1f));
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Green(), 0.1f));
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Blue(), 0.1f));
}

// Test short RGBA hex format
TEST_F(Quantity_ColorRGBATest, HexColorParsing_ShortRGBA)
{
  Quantity_ColorRGBA aColor;

  // Test short RGBA hex format: #RGBA
  bool aResult = Quantity_ColorRGBA::ColorFromHex("#F008", aColor, false);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Red(), 0.1f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Green(), 0.1f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Blue(), 0.1f));
  EXPECT_TRUE(IsNear(0.5f, aColor.Alpha(), 0.1f)); // 0x8 / 0xF ~= 0.533
}

// Test invalid hex formats
TEST_F(Quantity_ColorRGBATest, HexColorParsing_Invalid)
{
  Quantity_ColorRGBA aColor;

  // Empty string
  EXPECT_FALSE(Quantity_ColorRGBA::ColorFromHex("", aColor, true));

  // Invalid length
  EXPECT_FALSE(Quantity_ColorRGBA::ColorFromHex("#FFFF", aColor, true));

  // Invalid characters
  EXPECT_FALSE(Quantity_ColorRGBA::ColorFromHex("#GGGGGG", aColor, true));

  // RGBA format when alpha is disabled
  EXPECT_FALSE(Quantity_ColorRGBA::ColorFromHex("#FF0000FF", aColor, true));
}

// Test mixed case hex parsing
TEST_F(Quantity_ColorRGBATest, HexColorParsing_MixedCase)
{
  Quantity_ColorRGBA aColor;

  bool aResult = Quantity_ColorRGBA::ColorFromHex("#FfAa00", aColor, true);
  EXPECT_TRUE(aResult);
  EXPECT_TRUE(IsNear(1.0f, aColor.GetRGB().Red(), 0.01f));
  // AA = 170/255 = 0.667 sRGB, converts to ~0.402 linear RGB
  EXPECT_TRUE(IsNear(0.402f, aColor.GetRGB().Green(), 0.01f));
  EXPECT_TRUE(IsNear(0.0f, aColor.GetRGB().Blue(), 0.01f));
}

// Test specific hex value from our constant migration (regression test)
TEST_F(Quantity_ColorRGBATest, HexColorParsing_SpecificValues)
{
  Quantity_ColorRGBA aColor;

  // Test a color that would use our HEX_BASE constant (16)
  bool aResult = Quantity_ColorRGBA::ColorFromHex("#102030", aColor, true);
  EXPECT_TRUE(aResult);

  // Hex values are sRGB that get converted to linear RGB
  // 0x10 = 16/255 = 0.0627 sRGB -> 0.00518 linear RGB
  // 0x20 = 32/255 = 0.1255 sRGB -> 0.01444 linear RGB
  // 0x30 = 48/255 = 0.1882 sRGB -> 0.02956 linear RGB
  EXPECT_TRUE(IsNear(0.00518f, aColor.GetRGB().Red(), 0.0001f));
  EXPECT_TRUE(IsNear(0.01444f, aColor.GetRGB().Green(), 0.0001f));
  EXPECT_TRUE(IsNear(0.02956f, aColor.GetRGB().Blue(), 0.0001f));
}

// Test equality comparison
TEST_F(Quantity_ColorRGBATest, EqualityComparison)
{
  Quantity_ColorRGBA aColor1(0.5f, 0.6f, 0.7f, 0.8f);
  Quantity_ColorRGBA aColor2(0.5f, 0.6f, 0.7f, 0.8f);
  Quantity_ColorRGBA aColor3(0.5f, 0.6f, 0.7f, 0.9f); // Different alpha

  EXPECT_TRUE(aColor1.IsEqual(aColor2));
  EXPECT_FALSE(aColor1.IsEqual(aColor3));
}

// Test GetRGB and ChangeRGB
TEST_F(Quantity_ColorRGBATest, RGBAccess)
{
  Quantity_ColorRGBA aColor(0.2f, 0.4f, 0.6f, 0.8f);

  const Quantity_Color& aRGB = aColor.GetRGB();
  EXPECT_TRUE(IsNear(0.2f, aRGB.Red()));
  EXPECT_TRUE(IsNear(0.4f, aRGB.Green()));
  EXPECT_TRUE(IsNear(0.6f, aRGB.Blue()));

  aColor.ChangeRGB().SetValues(0.3f, 0.5f, 0.7f, Quantity_TOC_RGB);
  EXPECT_TRUE(IsNear(0.3f, aColor.GetRGB().Red()));
  EXPECT_TRUE(IsNear(0.5f, aColor.GetRGB().Green()));
  EXPECT_TRUE(IsNear(0.7f, aColor.GetRGB().Blue()));
  EXPECT_TRUE(IsNear(0.8f, aColor.Alpha())); // Alpha unchanged
}

// Test SetAlpha
TEST_F(Quantity_ColorRGBATest, SetAlpha)
{
  Quantity_ColorRGBA aColor(0.5f, 0.5f, 0.5f, 1.0f);
  EXPECT_TRUE(IsNear(1.0f, aColor.Alpha()));

  aColor.SetAlpha(0.5f);
  EXPECT_TRUE(IsNear(0.5f, aColor.Alpha()));

  // RGB should be unchanged
  EXPECT_TRUE(IsNear(0.5f, aColor.GetRGB().Red()));
  EXPECT_TRUE(IsNear(0.5f, aColor.GetRGB().Green()));
  EXPECT_TRUE(IsNear(0.5f, aColor.GetRGB().Blue()));
}

// Test edge cases
TEST_F(Quantity_ColorRGBATest, EdgeCases)
{
  // Fully transparent black
  Quantity_ColorRGBA aTransparent(0.0f, 0.0f, 0.0f, 0.0f);
  EXPECT_TRUE(IsNear(0.0f, aTransparent.Alpha()));

  // Fully opaque white
  Quantity_ColorRGBA aOpaque(1.0f, 1.0f, 1.0f, 1.0f);
  EXPECT_TRUE(IsNear(1.0f, aOpaque.Alpha()));
  EXPECT_TRUE(IsNear(1.0f, aOpaque.GetRGB().Red()));
}

// Test color component extraction in correct order (RGB_COMPONENT_LAST_INDEX)
TEST_F(Quantity_ColorRGBATest, ComponentOrder)
{
  Quantity_ColorRGBA aColor;

  // Parse a color where each component is distinct
  bool aResult = Quantity_ColorRGBA::ColorFromHex("#123456", aColor, true);
  EXPECT_TRUE(aResult);

  // Verify the order is correct: R=0x12, G=0x34, B=0x56
  // Hex values are sRGB that get converted to linear RGB
  // 0x12 = 18/255 = 0.0706 sRGB -> 0.00605 linear RGB
  // 0x34 = 52/255 = 0.2039 sRGB -> 0.03434 linear RGB
  // 0x56 = 86/255 = 0.3373 sRGB -> 0.09306 linear RGB
  float aExpectedR = 0.00605f;
  float aExpectedG = 0.03434f;
  float aExpectedB = 0.09306f;

  EXPECT_TRUE(IsNear(aExpectedR, aColor.GetRGB().Red(), 0.0001f));
  EXPECT_TRUE(IsNear(aExpectedG, aColor.GetRGB().Green(), 0.0001f));
  EXPECT_TRUE(IsNear(aExpectedB, aColor.GetRGB().Blue(), 0.0001f));
}
