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

#include <Quantity_Color.hxx>
#include <Quantity_NameOfColor.hxx>

#include <gtest/gtest.h>
#include <cmath>

// Test fixture for Quantity_Color tests
class Quantity_ColorTest : public testing::Test
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
TEST_F(Quantity_ColorTest, BasicConstruction)
{
  // Default constructor
  Quantity_Color aColor1;
  EXPECT_TRUE(IsNear(1.0, aColor1.Red()));   // YELLOW = RGB(1,1,0)
  EXPECT_TRUE(IsNear(1.0, aColor1.Green())); // YELLOW = RGB(1,1,0)
  EXPECT_TRUE(IsNear(0.0, aColor1.Blue()));  // YELLOW = RGB(1,1,0)

  // RGB constructor
  Quantity_Color aColor2(0.5, 0.6, 0.7, Quantity_TOC_RGB);
  EXPECT_TRUE(IsNear(0.5, aColor2.Red()));
  EXPECT_TRUE(IsNear(0.6, aColor2.Green()));
  EXPECT_TRUE(IsNear(0.7, aColor2.Blue()));

  // Named color constructor
  Quantity_Color aColor3(Quantity_NOC_RED);
  EXPECT_TRUE(IsNear(1.0, aColor3.Red()));
  EXPECT_TRUE(IsNear(0.0, aColor3.Green()));
  EXPECT_TRUE(IsNear(0.0, aColor3.Blue()));
}

// Test constexpr getters (compile-time evaluation capability)
TEST_F(Quantity_ColorTest, ConstexprGetters)
{
  const Quantity_Color aColor(0.3, 0.5, 0.7, Quantity_TOC_RGB);

  // These should work at compile-time with constexpr
  Standard_Real aR = aColor.Red();
  Standard_Real aG = aColor.Green();
  Standard_Real aB = aColor.Blue();

  EXPECT_TRUE(IsNear(0.3, aR));
  EXPECT_TRUE(IsNear(0.5, aG));
  EXPECT_TRUE(IsNear(0.7, aB));
}

// Test equality comparison (noexcept guarantee)
TEST_F(Quantity_ColorTest, EqualityComparison)
{
  Quantity_Color aColor1(0.5, 0.6, 0.7, Quantity_TOC_RGB);
  Quantity_Color aColor2(0.5, 0.6, 0.7, Quantity_TOC_RGB);
  Quantity_Color aColor3(0.5, 0.6, 0.8, Quantity_TOC_RGB);

  EXPECT_TRUE(aColor1.IsEqual(aColor2));
  EXPECT_TRUE(aColor1 == aColor2);
  EXPECT_FALSE(aColor1.IsDifferent(aColor2));
  EXPECT_FALSE(aColor1 != aColor2);

  EXPECT_FALSE(aColor1.IsEqual(aColor3));
  EXPECT_FALSE(aColor1 == aColor3);
  EXPECT_TRUE(aColor1.IsDifferent(aColor3));
  EXPECT_TRUE(aColor1 != aColor3);
}

// Test distance calculation (noexcept guarantee)
TEST_F(Quantity_ColorTest, DistanceCalculation)
{
  Quantity_Color aColor1(0.0, 0.0, 0.0, Quantity_TOC_RGB);
  Quantity_Color aColor2(0.3, 0.4, 0.0, Quantity_TOC_RGB);

  // Distance should be sqrt(0.3^2 + 0.4^2) = sqrt(0.09 + 0.16) = sqrt(0.25) = 0.5
  Standard_Real aDist = aColor1.Distance(aColor2);
  EXPECT_TRUE(IsNear(0.5, aDist));

  Standard_Real aSquareDist = aColor1.SquareDistance(aColor2);
  EXPECT_TRUE(IsNear(0.25, aSquareDist));
}

// Test sRGB to HLS conversion
TEST_F(Quantity_ColorTest, RGB_to_HLS_Conversion)
{
  // Pure red in sRGB
  Quantity_Color          aRed(Quantity_NOC_RED);
  NCollection_Vec3<float> aHLS = Quantity_Color::Convert_sRGB_To_HLS(aRed.Rgb());

  EXPECT_TRUE(IsNear(0.0, aHLS[0], 1.0)); // Hue for red should be ~0
  EXPECT_TRUE(IsNear(1.0, aHLS[1]));      // Lightness should be 1 (max value)
  EXPECT_TRUE(IsNear(1.0, aHLS[2]));      // Saturation should be 1 (fully saturated)

  // Gray (no saturation)
  Quantity_Color          aGray(0.5, 0.5, 0.5, Quantity_TOC_RGB);
  NCollection_Vec3<float> aHLS_Gray = Quantity_Color::Convert_sRGB_To_HLS(aGray.Rgb());

  EXPECT_TRUE(IsNear(0.5, aHLS_Gray[1])); // Lightness
  EXPECT_TRUE(IsNear(0.0, aHLS_Gray[2])); // Saturation should be 0 for gray
}

// Test Linear RGB to CIE Lab conversion (uses new constexpr constants)
TEST_F(Quantity_ColorTest, LinearRGB_to_Lab_Conversion)
{
  // White should convert to L=100, a=0, b=0 in Lab
  Quantity_Color          aWhite(1.0, 1.0, 1.0, Quantity_TOC_RGB);
  NCollection_Vec3<float> aLab = Quantity_Color::Convert_LinearRGB_To_Lab(aWhite.Rgb());

  EXPECT_TRUE(IsNear(100.0, aLab[0], 1.0)); // L should be near 100
  EXPECT_TRUE(IsNear(0.0, aLab[1], 5.0));   // a should be near 0
  EXPECT_TRUE(IsNear(0.0, aLab[2], 5.0));   // b should be near 0

  // Black should convert to L=0
  Quantity_Color          aBlack(0.0, 0.0, 0.0, Quantity_TOC_RGB);
  NCollection_Vec3<float> aLabBlack = Quantity_Color::Convert_LinearRGB_To_Lab(aBlack.Rgb());

  EXPECT_TRUE(IsNear(0.0, aLabBlack[0], 1.0)); // L should be 0
}

// Test Lab to Lch conversion
TEST_F(Quantity_ColorTest, Lab_to_Lch_Conversion)
{
  // Test with known Lab values
  NCollection_Vec3<float> aLab(50.0f, 25.0f, 25.0f);
  NCollection_Vec3<float> aLch = Quantity_Color::Convert_Lab_To_Lch(aLab);

  EXPECT_TRUE(IsNear(50.0, aLch[0])); // L should be preserved

  // C (chroma) should be sqrt(25^2 + 25^2) = sqrt(1250) ~= 35.36
  EXPECT_TRUE(IsNear(35.36, aLch[1], 0.1));

  // H (hue) should be atan2(25, 25) * 180/pi = 45 degrees
  EXPECT_TRUE(IsNear(45.0, aLch[2], 1.0));
}

// Test Lch to Lab conversion (round-trip)
TEST_F(Quantity_ColorTest, Lch_to_Lab_RoundTrip)
{
  NCollection_Vec3<float> aLab1(50.0f, 25.0f, 25.0f);
  NCollection_Vec3<float> aLch  = Quantity_Color::Convert_Lab_To_Lch(aLab1);
  NCollection_Vec3<float> aLab2 = Quantity_Color::Convert_Lch_To_Lab(aLch);

  EXPECT_TRUE(IsNear(aLab1[0], aLab2[0], 0.01));
  EXPECT_TRUE(IsNear(aLab1[1], aLab2[1], 0.01));
  EXPECT_TRUE(IsNear(aLab1[2], aLab2[2], 0.01));
}

// Test Lab to RGB conversion (round-trip validation)
TEST_F(Quantity_ColorTest, Lab_to_RGB_RoundTrip)
{
  Quantity_Color          aOriginal(0.5, 0.6, 0.7, Quantity_TOC_RGB);
  NCollection_Vec3<float> aLab = Quantity_Color::Convert_LinearRGB_To_Lab(aOriginal.Rgb());
  NCollection_Vec3<float> aRGB = Quantity_Color::Convert_Lab_To_LinearRGB(aLab);

  EXPECT_TRUE(IsNear(aOriginal.Red(), aRGB[0], 0.01));
  EXPECT_TRUE(IsNear(aOriginal.Green(), aRGB[1], 0.01));
  EXPECT_TRUE(IsNear(aOriginal.Blue(), aRGB[2], 0.01));
}

// Test DeltaE2000 color difference (uses Epsilon() function - regression test for bug fix)
TEST_F(Quantity_ColorTest, DeltaE2000_Calculation)
{
  // Same color should have DeltaE = 0
  Quantity_Color aColor1(0.5, 0.6, 0.7, Quantity_TOC_RGB);
  Quantity_Color aColor2(0.5, 0.6, 0.7, Quantity_TOC_RGB);

  Standard_Real aDeltaE = aColor1.DeltaE2000(aColor2);
  EXPECT_TRUE(IsNear(0.0, aDeltaE, 0.01));

  // Different colors should have non-zero DeltaE
  Quantity_Color aColor3(0.3, 0.4, 0.5, Quantity_TOC_RGB);
  Standard_Real  aDeltaE2 = aColor1.DeltaE2000(aColor3);
  EXPECT_GT(aDeltaE2, 0.0);
}

// Test named color conversion
TEST_F(Quantity_ColorTest, NamedColors)
{
  // Test a few standard colors
  Quantity_Color aRed(Quantity_NOC_RED);
  EXPECT_TRUE(IsNear(1.0, aRed.Red()));
  EXPECT_TRUE(IsNear(0.0, aRed.Green()));
  EXPECT_TRUE(IsNear(0.0, aRed.Blue()));

  Quantity_Color aGreen(Quantity_NOC_GREEN);
  EXPECT_TRUE(IsNear(0.0, aGreen.Red()));
  EXPECT_GT(aGreen.Green(), 0.5); // Green should be significant
  EXPECT_TRUE(IsNear(0.0, aGreen.Blue()));

  Quantity_Color aBlue(Quantity_NOC_BLUE);
  EXPECT_TRUE(IsNear(0.0, aBlue.Red()));
  EXPECT_TRUE(IsNear(0.0, aBlue.Green()));
  EXPECT_TRUE(IsNear(1.0, aBlue.Blue()));
}

// Test SetValues and modification
TEST_F(Quantity_ColorTest, SetValues)
{
  Quantity_Color aColor;

  aColor.SetValues(0.2, 0.4, 0.6, Quantity_TOC_RGB);
  EXPECT_TRUE(IsNear(0.2, aColor.Red()));
  EXPECT_TRUE(IsNear(0.4, aColor.Green()));
  EXPECT_TRUE(IsNear(0.6, aColor.Blue()));

  aColor.SetValues(Quantity_NOC_YELLOW);
  EXPECT_TRUE(IsNear(1.0, aColor.Red()));
  EXPECT_TRUE(IsNear(1.0, aColor.Green()));
  EXPECT_TRUE(IsNear(0.0, aColor.Blue()));
}

// Test HLS values extraction
TEST_F(Quantity_ColorTest, HLS_Extraction)
{
  Quantity_Color aRed(Quantity_NOC_RED);

  // For pure red, hue should be ~0, saturation should be 1, lightness should be 1
  Standard_Real aHue   = aRed.Hue();
  Standard_Real aLight = aRed.Light();
  Standard_Real aSat   = aRed.Saturation();

  EXPECT_TRUE(IsNear(0.0, aHue, 5.0) || IsNear(360.0, aHue, 5.0)); // Hue wraps around
  EXPECT_TRUE(IsNear(1.0, aLight, 0.01));
  EXPECT_TRUE(IsNear(1.0, aSat, 0.01));
}

// Test thread-safety of Epsilon getter/setter
TEST_F(Quantity_ColorTest, EpsilonThreadSafety)
{
  Standard_Real aOriginalEpsilon = Quantity_Color::Epsilon();

  // Set new epsilon
  Quantity_Color::SetEpsilon(0.0002);
  EXPECT_TRUE(IsNear(0.0002, Quantity_Color::Epsilon()));

  // Restore original
  Quantity_Color::SetEpsilon(aOriginalEpsilon);
  EXPECT_TRUE(IsNear(aOriginalEpsilon, Quantity_Color::Epsilon()));
}

// Test color name string conversion
TEST_F(Quantity_ColorTest, ColorNameString)
{
  Standard_CString aRedName = Quantity_Color::StringName(Quantity_NOC_RED);
  EXPECT_STREQ("RED", aRedName);

  Standard_CString aBlueName = Quantity_Color::StringName(Quantity_NOC_BLUE);
  EXPECT_STREQ("BLUE", aBlueName);
}

// Test edge cases and boundary conditions
TEST_F(Quantity_ColorTest, EdgeCases)
{
  // Test with zero values
  Quantity_Color aBlack(0.0, 0.0, 0.0, Quantity_TOC_RGB);
  EXPECT_TRUE(IsNear(0.0, aBlack.Red()));
  EXPECT_TRUE(IsNear(0.0, aBlack.Green()));
  EXPECT_TRUE(IsNear(0.0, aBlack.Blue()));

  // Test with max values
  Quantity_Color aWhite(1.0, 1.0, 1.0, Quantity_TOC_RGB);
  EXPECT_TRUE(IsNear(1.0, aWhite.Red()));
  EXPECT_TRUE(IsNear(1.0, aWhite.Green()));
  EXPECT_TRUE(IsNear(1.0, aWhite.Blue()));

  // Test equality with epsilon tolerance
  Quantity_Color aColor1(0.5, 0.5, 0.5, Quantity_TOC_RGB);
  Quantity_Color aColor2(0.50001, 0.50001, 0.50001, Quantity_TOC_RGB);
  EXPECT_TRUE(aColor1.IsEqual(aColor2)); // Should be equal within epsilon
}
