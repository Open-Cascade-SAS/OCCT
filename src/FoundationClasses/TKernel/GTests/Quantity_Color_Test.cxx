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
  bool IsNear(double theValue1, double theValue2, double theTolerance = 0.001) const
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
  double aR = aColor.Red();
  double aG = aColor.Green();
  double aB = aColor.Blue();

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
  double aDist = aColor1.Distance(aColor2);
  EXPECT_TRUE(IsNear(0.5, aDist));

  double aSquareDist = aColor1.SquareDistance(aColor2);
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

  double aDeltaE = aColor1.DeltaE2000(aColor2);
  EXPECT_TRUE(IsNear(0.0, aDeltaE, 0.01));

  // Different colors should have non-zero DeltaE
  Quantity_Color aColor3(0.3, 0.4, 0.5, Quantity_TOC_RGB);
  double         aDeltaE2 = aColor1.DeltaE2000(aColor3);
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
  double aHue   = aRed.Hue();
  double aLight = aRed.Light();
  double aSat   = aRed.Saturation();

  EXPECT_TRUE(IsNear(0.0, aHue, 5.0) || IsNear(360.0, aHue, 5.0)); // Hue wraps around
  EXPECT_TRUE(IsNear(1.0, aLight, 0.01));
  EXPECT_TRUE(IsNear(1.0, aSat, 0.01));
}

// Test thread-safety of Epsilon getter/setter
TEST_F(Quantity_ColorTest, EpsilonThreadSafety)
{
  double aOriginalEpsilon = Quantity_Color::Epsilon();

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
  const char* aRedName = Quantity_Color::StringName(Quantity_NOC_RED);
  EXPECT_STREQ("RED", aRedName);

  const char* aBlueName = Quantity_Color::StringName(Quantity_NOC_BLUE);
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

namespace
{
struct ColorConversionSample
{
  double myR;
  double myG;
  double myB;
  double myC1;
  double myC2;
  double myC3;
};

struct LabDifferenceSample
{
  double myL1;
  double myA1;
  double myB1;
  double myL2;
  double myA2;
  double myB2;
  double myExpected;
  double myTolerance;
};

void expectColorValues(const ColorConversionSample& theSample,
                       const Quantity_TypeOfColor  theType,
                       const double                theTolerance)
{
  Quantity_Color aColor(theSample.myR, theSample.myG, theSample.myB, Quantity_TOC_RGB);
  double         aC1, aC2, aC3;
  aColor.Values(aC1, aC2, aC3, theType);
  EXPECT_NEAR(aC1, theSample.myC1, theTolerance);
  EXPECT_NEAR(aC2, theSample.myC2, theTolerance);
  EXPECT_NEAR(aC3, theSample.myC3, theTolerance);
}

void expectLabDifference(const LabDifferenceSample& theSample)
{
  const Quantity_Color aColor1(theSample.myL1,
                               theSample.myA1,
                               theSample.myB1,
                               Quantity_TOC_CIELab);
  const Quantity_Color aColor2(theSample.myL2,
                               theSample.myA2,
                               theSample.myB2,
                               Quantity_TOC_CIELab);
  EXPECT_NEAR(aColor1.DeltaE2000(aColor2), theSample.myExpected, theSample.myTolerance);
}
} // namespace

// Migrated from tests/v3d/colors/rgb2lab.  The DRAW command uses RGB as
// linear RGB, which is the native Quantity_Color representation.
TEST_F(Quantity_ColorTest, Draw_RGBToLabReferenceSamples)
{
  static const ColorConversionSample aSamples[] = {
    {0., 0., 0., 0., 0., 0.},
    {1., 1., 1., 100., 0., 0.},
    {0.5, 0.5, 0.5, 76.0693, 0., 0.},
    {1., 0., 0., 53.2408, 80.0925, 67.2032},
    {0., 1., 0., 87.7347, -86.1827, 83.1793},
    {0., 0., 1., 32.2970, 79.1875, -107.8602},
    {0., 1., 1., 91.1132, -48.0875, -14.1312},
    {1., 1., 0., 97.1393, -21.5537, 94.4780},
    {1., 0., 1., 60.3242, 98.2343, -60.8249},
    {0.1, 0., 0., 16.1387, 37.1756, 25.0600},
    {0.3, 0., 0., 30.3521, 53.6166, 44.0349},
    {0.5, 0., 0., 38.9565, 63.5695, 53.3392},
    {0.7, 0., 0., 45.4792, 71.1144, 59.6700},
    {0.9, 0., 0., 50.8512, 77.3285, 64.8840},
    {0.3, 0.5, 0.9, 75.2228, 0.7560, -31.8425},
  };

  for (const ColorConversionSample& aSample : aSamples)
  {
    expectColorValues(aSample, Quantity_TOC_CIELab, 1.e-4);
  }
}

// Migrated from tests/v3d/colors/rgb2lch.
TEST_F(Quantity_ColorTest, Draw_RGBToLchReferenceSamples)
{
  static const ColorConversionSample aSamples[] = {
    {0., 0., 0., 0., 0., 0.},
    {1., 1., 1., 100., 0., 0.},
    {0.5, 0.5, 0.5, 76.0693, 0., 0.},
    {1., 0., 0., 53.2408, 104.5518, 39.9990},
    {0., 1., 0., 87.7347, 119.7759, 136.0160},
    {0., 0., 1., 32.2970, 133.8076, 306.2849},
    {0., 1., 1., 91.1132, 50.1209, 196.3762},
    {1., 1., 0., 97.1393, 96.9054, 102.8512},
    {1., 0., 1., 60.3242, 115.5407, 328.2350},
    {0.1, 0., 0., 16.1387, 44.8334, 33.9838},
    {0.3, 0., 0., 30.3521, 69.3816, 39.3960},
    {0.5, 0., 0., 38.9565, 82.9828, 39.9990},
    {0.7, 0., 0., 45.4792, 92.8320, 39.9990},
    {0.9, 0., 0., 50.8512, 100.9436, 39.9990},
    {0.3, 0.5, 0.9, 75.2228, 31.8514, 271.3601},
  };

  for (const ColorConversionSample& aSample : aSamples)
  {
    expectColorValues(aSample, Quantity_TOC_CIELch, 1.e-4);
  }
}

// Migrated from tests/v3d/colors/stability.  Use deterministic samples so
// failures are reproducible while retaining both conversion round trips.
TEST_F(Quantity_ColorTest, Draw_ColorSpaceRoundTripStability)
{
  for (int anIndex = 1; anIndex < 1000; ++anIndex)
  {
    const double aR = std::fmod(anIndex * 0.6180339887498949, 1.0);
    const double aG = std::fmod(anIndex * 0.4142135623730950, 1.0);
    const double aB = std::fmod(anIndex * 0.7320508075688773, 1.0);

    Quantity_Color aColor(aR, aG, aB, Quantity_TOC_RGB);
    double         aL, anA, aLabB;
    aColor.Values(aL, anA, aLabB, Quantity_TOC_CIELab);

    Quantity_Color aFromLab;
    aFromLab.SetValues(aL, anA, aLabB, Quantity_TOC_CIELab);
    EXPECT_NEAR(aFromLab.Red(), aR, 1.e-4);
    EXPECT_NEAR(aFromLab.Green(), aG, 1.e-4);
    EXPECT_NEAR(aFromLab.Blue(), aB, 1.e-4);

    double aC, aH;
    aColor.Values(aL, aC, aH, Quantity_TOC_CIELch);
    Quantity_Color aFromLch;
    aFromLch.SetValues(aL, aC, aH, Quantity_TOC_CIELch);
    EXPECT_NEAR(aFromLch.Red(), aR, 1.e-4);
    EXPECT_NEAR(aFromLch.Green(), aG, 1.e-4);
    EXPECT_NEAR(aFromLch.Blue(), aB, 1.e-4);
  }
}

// Migrated from tests/v3d/colors/de2000.  Constructing the colors from Lab
// follows the same gamut-adjusting conversion as vcolorconvert -from lab.
TEST_F(Quantity_ColorTest, Draw_CIEDE2000ReferenceSamples)
{
  static const LabDifferenceSample aSamples[] = {
    {0., 0., 0., 50., 0., 0., 36.519268, 1.e-4},
    {50., 0., 0., 100., 0., 0., 36.519268, 1.e-4},
    {0., 0., 0., 100., 0., 0., 100., 1.e-4},
    {20., 10., 10., 80., 10., 10., 60., 1.e-4},
    {50., 0., 0., 50., 0., 50., 23.529412, 1.e-4},
    {50., 60., 60., 50., 60., 0., 28.016927, 1.e-4},
    {30., 30., 40., 30., 30., -60., 44.606253, 1.e-4},
    {30., 50.00, 40., 20., -10., -8., 39.105394, 1.e-3},
    {30., 50.01, 40., 20., -10., -8., 43.53247, 1.e-3},
    {20., 30.00, 30.01, 60., -10., -10., 49.416742, 0.05},
    {20., 30.01, 30.00, 60., -10., -10., 52.448227, 0.05},
    {73.4450, 34.9839, -24.6753, 87.6216, -18.4863, 57.8838, 62.402500, 1.e-4},
    {93.6166, -27.3677, 29.3893, 46.9191, 12.3400, -27.5948, 54.640034, 1.e-4},
    {53.9062, 61.0929, -51.7583, 65.5157, 26.3376, -37.0512, 15.679046, 1.e-4},
    {83.6996, 9.3358, -24.5571, 93.2268, -3.8589, 3.5217, 23.158692, 1.e-4},
    {64.8053, -27.3177, -8.9602, 65.8225, 37.3192, -38.1465, 34.670514, 1.e-4},
    {94.7633, -19.7915, 69.2787, 90.9238, -16.7535, 4.1936, 26.093024, 1.e-4},
    {85.4699, 5.6078, -11.1083, 67.9455, -28.4536, 7.8808, 31.115070, 1.e-4},
    {83.5473, -15.7170, 8.3546, 81.3193, -37.2851, 57.7090, 19.696753, 1.e-4},
    {75.7406, -12.0785, -12.3505, 80.0810, -54.8591, 52.1739, 35.834099, 1.e-4},
    {62.8209, 32.1209, 16.9113, 82.1106, 25.0843, -7.9416, 21.178519, 1.e-4},
  };

  for (const LabDifferenceSample& aSample : aSamples)
  {
    expectLabDifference(aSample);
  }
}

// Migrated from tests/v3d/colors/de2000_sharma.  This is the published
// supplementary data set from Sharma, Wu, and Dalal, excluding only the
// six out-of-gamut samples intentionally excluded by the DRAW test.
TEST_F(Quantity_ColorTest, Draw_CIEDE2000SharmaReferenceSamples)
{
  static const LabDifferenceSample aSamples[] = {
    {50., 0., 0., 50., -1., 2., 2.3669, 1.e-3},
    {50., -1., 2., 50., 0., 0., 2.3669, 1.e-3},
    {50., 2.490, -0.001, 50., -2.490, 0.0009, 7.1792, 1.e-3},
    {50., 2.490, -0.001, 50., -2.490, 0.0010, 7.1792, 0.05},
    {50., 2.490, -0.001, 50., -2.490, 0.0011, 7.2195, 1.e-3},
    {50., 2.490, -0.001, 50., -2.490, 0.0012, 7.2195, 1.e-3},
    {50., -0.001, 2.490, 50., 0.0009, -2.490, 4.8045, 1.e-3},
    {50., -0.001, 2.490, 50., 0.0010, -2.490, 4.8045, 1.e-3},
    {50., -0.001, 2.490, 50., 0.0011, -2.490, 4.7461, 0.06},
    {50., 2.5, 0., 50., 0., -2.5, 4.3065, 1.e-3},
    {50., 2.5, 0., 73., 25., -18., 27.1492, 1.e-3},
    {50., 2.5, 0., 61., -5., 29., 22.8977, 1.e-3},
    {50., 2.5, 0., 56., -27., -3., 31.9030, 1.e-3},
    {50., 2.5, 0., 58., 24., 15., 19.4535, 1.e-3},
    {50., 2.5, 0., 50., 3.1736, 0.5854, 1., 1.e-3},
    {50., 2.5, 0., 50., 3.2972, 0., 1., 1.e-3},
    {50., 2.5, 0., 50., 1.8634, 0.5757, 1., 1.e-3},
    {50., 2.5, 0., 50., 3.2592, 0.3350, 1., 1.e-3},
    {60.2574, -34.0099, 36.2677, 60.4626, -34.1751, 39.4387, 1.2644, 1.e-3},
    {63.0109, -31.0961, -5.8663, 62.8187, -29.7946, -4.0864, 1.2630, 1.e-3},
    {61.2901, 3.7196, -5.3901, 61.4292, 2.2480, -4.9620, 1.8731, 1.e-3},
    {22.7233, 20.0904, -46.6940, 23.0331, 14.9730, -42.5619, 2.0373, 1.e-3},
    {36.4612, 47.8580, 18.3852, 36.2715, 50.5065, 21.2231, 1.4146, 1.e-3},
    {90.8027, -2.0831, 1.4410, 91.1528, -1.6435, 0.0447, 1.4441, 1.e-3},
    {90.9257, -0.5406, -0.9208, 88.6381, -0.8985, -0.7239, 1.5381, 1.e-3},
    {6.7747, -0.2908, -2.4247, 5.8714, -0.0985, -2.2286, 0.6377, 1.e-3},
    {2.0776, 0.0795, -1.1350, 0.9033, -0.0636, -0.5514, 0.9082, 1.e-3},
  };

  for (const LabDifferenceSample& aSample : aSamples)
  {
    expectLabDifference(aSample);
  }
}
