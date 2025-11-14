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

#include <STEPConstruct_RenderingProperties.hxx>

#include <STEPConstruct_Styles.hxx>
#include <StepVisual_SurfaceStyleRenderingWithProperties.hxx>
#include <StepVisual_HArray1OfRenderingPropertiesSelect.hxx>
#include <StepVisual_SurfaceStyleTransparent.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular.hxx>
#include <StepVisual_ColourRgb.hxx>
#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <TCollection_HAsciiString.hxx>

#include <gtest/gtest.h>

// Test fixture for STEPConstruct_RenderingProperties tests
class STEPConstruct_RenderingPropertiesTest : public ::testing::Test
{
protected:
  //! Set up function called before each test
  void SetUp() override
  {
    // Create some common colors and values for testing
    mySurfaceColor     = Quantity_Color(0.8, 0.5, 0.2, Quantity_TOC_RGB);
    myTransparency     = 0.25;
    myAmbientFactor    = 0.3;
    myDiffuseFactor    = 1.0;
    mySpecularFactor   = 0.8;
    mySpecularExponent = 45.0;
    mySpecularColor    = Quantity_Color(0.9, 0.9, 0.9, Quantity_TOC_RGB);
  }

  //! Helper function to create a STEP rendering properties object for testing
  Handle(StepVisual_SurfaceStyleRenderingWithProperties) CreateStepRenderingProperties()
  {
    // Create the surface color
    Handle(TCollection_HAsciiString) aColorName = new TCollection_HAsciiString("");
    Handle(StepVisual_Colour) aSurfaceColor     = STEPConstruct_Styles::EncodeColor(mySurfaceColor);

    // Create transparency property
    Handle(StepVisual_SurfaceStyleTransparent) aTransp = new StepVisual_SurfaceStyleTransparent();
    aTransp->Init(myTransparency);

    // Create specular color
    Handle(StepVisual_Colour) aSpecColor = STEPConstruct_Styles::EncodeColor(mySpecularColor);

    // Create reflectance model
    Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular) aReflectance =
      new StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular();
    aReflectance->Init(myAmbientFactor,
                       myDiffuseFactor,
                       mySpecularFactor,
                       mySpecularExponent,
                       aSpecColor);

    // Create the properties array with two entries: transparency and reflectance
    Handle(StepVisual_HArray1OfRenderingPropertiesSelect) aProps =
      new StepVisual_HArray1OfRenderingPropertiesSelect(1, 2);

    StepVisual_RenderingPropertiesSelect aRps1, aRps2;
    aRps1.SetValue(aTransp);
    aRps2.SetValue(aReflectance);

    aProps->SetValue(1, aRps1);
    aProps->SetValue(2, aRps2);

    // Create and return the final object
    Handle(StepVisual_SurfaceStyleRenderingWithProperties) aResult =
      new StepVisual_SurfaceStyleRenderingWithProperties();
    aResult->Init(StepVisual_ssmNormalShading, aSurfaceColor, aProps);

    return aResult;
  }

  //! Helper function to create a common material for testing
  XCAFDoc_VisMaterialCommon CreateMaterial()
  {
    XCAFDoc_VisMaterialCommon aMaterial;

    // Set basic properties
    aMaterial.DiffuseColor = mySurfaceColor;
    aMaterial.Transparency = static_cast<Standard_ShortReal>(myTransparency);

    // Calculate ambient color based on ambient factor
    aMaterial.AmbientColor = Quantity_Color(mySurfaceColor.Red() * myAmbientFactor,
                                            mySurfaceColor.Green() * myAmbientFactor,
                                            mySurfaceColor.Blue() * myAmbientFactor,
                                            Quantity_TOC_RGB);

    // Set specular properties
    aMaterial.SpecularColor = mySpecularColor;
    aMaterial.Shininess     = (Standard_ShortReal)(mySpecularExponent / 128.0);

    // Mark as defined
    aMaterial.IsDefined = Standard_True;

    return aMaterial;
  }

  //! Compare two colors with tolerance
  Standard_Boolean AreColorsEqual(const Quantity_Color& theC1,
                                  const Quantity_Color& theC2,
                                  const Standard_Real   theTol = 0.01)
  {
    return (std::abs(theC1.Red() - theC2.Red()) <= theTol)
           && (std::abs(theC1.Green() - theC2.Green()) <= theTol)
           && (std::abs(theC1.Blue() - theC2.Blue()) <= theTol);
  }

  // Test member variables
  Quantity_Color mySurfaceColor;     //!< Surface color for testing
  Quantity_Color mySpecularColor;    //!< Specular color for testing
  Standard_Real  myTransparency;     //!< Transparency value for testing
  Standard_Real  myAmbientFactor;    //!< Ambient reflectance factor for testing
  Standard_Real  myDiffuseFactor;    //!< Diffuse reflectance factor for testing
  Standard_Real  mySpecularFactor;   //!< Specular reflectance factor for testing
  Standard_Real  mySpecularExponent; //!< Specular exponent value for testing
};

// Test default constructor
TEST_F(STEPConstruct_RenderingPropertiesTest, DefaultConstructor)
{
  STEPConstruct_RenderingProperties aProps;

  EXPECT_FALSE(aProps.IsDefined());
  EXPECT_FALSE(aProps.IsAmbientReflectanceDefined());
  EXPECT_FALSE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_FALSE(aProps.IsSpecularReflectanceDefined());
  EXPECT_FALSE(aProps.IsSpecularExponentDefined());
  EXPECT_FALSE(aProps.IsSpecularColourDefined());

  EXPECT_EQ(aProps.Transparency(), 0.0);
  EXPECT_EQ(aProps.RenderingMethod(), StepVisual_ssmNormalShading);
}

// Test RGBA color constructor
TEST_F(STEPConstruct_RenderingPropertiesTest, RGBAConstructor)
{
  // Create an RGBA color with alpha = 0.75 (transparency = 0.25)
  Quantity_ColorRGBA aRgba(mySurfaceColor, 0.75);

  // Create rendering properties from RGBA
  STEPConstruct_RenderingProperties aProps(aRgba);

  EXPECT_TRUE(aProps.IsDefined());
  EXPECT_FALSE(aProps.IsAmbientReflectanceDefined());
  EXPECT_FALSE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_FALSE(aProps.IsSpecularReflectanceDefined());

  EXPECT_TRUE(AreColorsEqual(aProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aProps.Transparency(), 0.25, 0.001);
  EXPECT_EQ(aProps.RenderingMethod(), StepVisual_ssmNormalShading);
}

// Test StepVisual_SurfaceStyleRenderingWithProperties constructor
TEST_F(STEPConstruct_RenderingPropertiesTest, StepRenderingPropertiesConstructor)
{
  Handle(StepVisual_SurfaceStyleRenderingWithProperties) aStepProps =
    CreateStepRenderingProperties();

  STEPConstruct_RenderingProperties aProps(aStepProps);

  EXPECT_TRUE(aProps.IsDefined());
  EXPECT_TRUE(aProps.IsAmbientReflectanceDefined());
  EXPECT_TRUE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_TRUE(aProps.IsSpecularReflectanceDefined());
  EXPECT_TRUE(aProps.IsSpecularExponentDefined());
  EXPECT_TRUE(aProps.IsSpecularColourDefined());

  EXPECT_TRUE(AreColorsEqual(aProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aProps.Transparency(), myTransparency, 0.001);
  EXPECT_NEAR(aProps.AmbientReflectance(), myAmbientFactor, 0.001);
  EXPECT_NEAR(aProps.DiffuseReflectance(), myDiffuseFactor, 0.001);
  EXPECT_NEAR(aProps.SpecularReflectance(), mySpecularFactor, 0.001);
  EXPECT_NEAR(aProps.SpecularExponent(), mySpecularExponent, 0.001);
  EXPECT_TRUE(AreColorsEqual(aProps.SpecularColour(), mySpecularColor));
  EXPECT_EQ(aProps.RenderingMethod(), StepVisual_ssmNormalShading);
}

// Test XCAFDoc_VisMaterialCommon constructor
TEST_F(STEPConstruct_RenderingPropertiesTest, MaterialConstructor)
{
  XCAFDoc_VisMaterialCommon aMaterial = CreateMaterial();

  STEPConstruct_RenderingProperties aProps(aMaterial);

  EXPECT_TRUE(aProps.IsDefined());
  EXPECT_TRUE(aProps.IsAmbientReflectanceDefined());
  EXPECT_TRUE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_TRUE(aProps.IsSpecularReflectanceDefined());
  EXPECT_TRUE(aProps.IsSpecularExponentDefined());
  EXPECT_TRUE(aProps.IsSpecularColourDefined());

  EXPECT_TRUE(AreColorsEqual(aProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aProps.Transparency(), myTransparency, 0.001);
  EXPECT_NEAR(aProps.AmbientReflectance(), myAmbientFactor, 0.02); // Slightly larger tolerance
  EXPECT_NEAR(aProps.DiffuseReflectance(), 1.0, 0.001);
}

// Test setting reflectance properties
TEST_F(STEPConstruct_RenderingPropertiesTest, SetReflectanceProperties)
{
  STEPConstruct_RenderingProperties aProps;

  // Initialize with basic color and transparency
  aProps.Init(mySurfaceColor, myTransparency);
  EXPECT_TRUE(aProps.IsDefined());

  // Set ambient reflectance
  aProps.SetAmbientReflectance(myAmbientFactor);
  EXPECT_TRUE(aProps.IsAmbientReflectanceDefined());
  EXPECT_NEAR(aProps.AmbientReflectance(), myAmbientFactor, 0.001);
  EXPECT_FALSE(aProps.IsDiffuseReflectanceDefined());

  // Set ambient and diffuse reflectance
  aProps.SetAmbientAndDiffuseReflectance(myAmbientFactor, myDiffuseFactor);
  EXPECT_TRUE(aProps.IsAmbientReflectanceDefined());
  EXPECT_TRUE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_NEAR(aProps.AmbientReflectance(), myAmbientFactor, 0.001);
  EXPECT_NEAR(aProps.DiffuseReflectance(), myDiffuseFactor, 0.001);
  EXPECT_FALSE(aProps.IsSpecularReflectanceDefined());

  // Set all reflectance properties
  aProps.SetAmbientDiffuseAndSpecularReflectance(myAmbientFactor,
                                                 myDiffuseFactor,
                                                 mySpecularFactor,
                                                 mySpecularExponent,
                                                 mySpecularColor);

  EXPECT_TRUE(aProps.IsAmbientReflectanceDefined());
  EXPECT_TRUE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_TRUE(aProps.IsSpecularReflectanceDefined());
  EXPECT_TRUE(aProps.IsSpecularExponentDefined());
  EXPECT_TRUE(aProps.IsSpecularColourDefined());

  EXPECT_NEAR(aProps.AmbientReflectance(), myAmbientFactor, 0.001);
  EXPECT_NEAR(aProps.DiffuseReflectance(), myDiffuseFactor, 0.001);
  EXPECT_NEAR(aProps.SpecularReflectance(), mySpecularFactor, 0.001);
  EXPECT_NEAR(aProps.SpecularExponent(), mySpecularExponent, 0.001);
  EXPECT_TRUE(AreColorsEqual(aProps.SpecularColour(), mySpecularColor));
}

// Test creating STEP rendering properties
TEST_F(STEPConstruct_RenderingPropertiesTest, CreateRenderingProperties)
{
  STEPConstruct_RenderingProperties aProps;
  aProps.Init(mySurfaceColor, myTransparency);
  aProps.SetAmbientDiffuseAndSpecularReflectance(myAmbientFactor,
                                                 myDiffuseFactor,
                                                 mySpecularFactor,
                                                 mySpecularExponent,
                                                 mySpecularColor);

  Handle(StepVisual_SurfaceStyleRenderingWithProperties) aStepProps =
    aProps.CreateRenderingProperties();

  ASSERT_FALSE(aStepProps.IsNull());
  EXPECT_EQ(aStepProps->RenderingMethod(), StepVisual_ssmNormalShading);

  // Verify properties through re-parsing
  STEPConstruct_RenderingProperties aParsedProps(aStepProps);

  EXPECT_TRUE(aParsedProps.IsDefined());
  EXPECT_TRUE(AreColorsEqual(aParsedProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aParsedProps.Transparency(), myTransparency, 0.001);
  EXPECT_NEAR(aParsedProps.AmbientReflectance(), myAmbientFactor, 0.001);
  EXPECT_NEAR(aParsedProps.DiffuseReflectance(), myDiffuseFactor, 0.001);
  EXPECT_NEAR(aParsedProps.SpecularReflectance(), mySpecularFactor, 0.001);
  EXPECT_NEAR(aParsedProps.SpecularExponent(), mySpecularExponent, 0.001);
  EXPECT_TRUE(AreColorsEqual(aParsedProps.SpecularColour(), mySpecularColor));
}

// Test creating XCAFDoc_VisMaterialCommon
TEST_F(STEPConstruct_RenderingPropertiesTest, CreateXCAFMaterial)
{
  STEPConstruct_RenderingProperties aProps;
  aProps.Init(mySurfaceColor, myTransparency);
  aProps.SetAmbientDiffuseAndSpecularReflectance(myAmbientFactor,
                                                 myDiffuseFactor,
                                                 mySpecularFactor,
                                                 mySpecularExponent,
                                                 mySpecularColor);

  XCAFDoc_VisMaterialCommon aMaterial = aProps.CreateXCAFMaterial();
  EXPECT_TRUE(aMaterial.IsDefined);

  // Check basic properties
  EXPECT_TRUE(AreColorsEqual(aMaterial.DiffuseColor, mySurfaceColor));
  EXPECT_NEAR(aMaterial.Transparency, myTransparency, 0.001);

  // Check calculated ambient color
  Quantity_Color anExpectedAmbient(mySurfaceColor.Red() * myAmbientFactor,
                                   mySurfaceColor.Green() * myAmbientFactor,
                                   mySurfaceColor.Blue() * myAmbientFactor,
                                   Quantity_TOC_RGB);
  EXPECT_TRUE(AreColorsEqual(aMaterial.AmbientColor, anExpectedAmbient));

  // Check specular properties
  EXPECT_TRUE(AreColorsEqual(aMaterial.SpecularColor, mySpecularColor));
  EXPECT_NEAR(aMaterial.Shininess, mySpecularExponent / 128.0, 0.01);
}

// Test bidirectional conversion
TEST_F(STEPConstruct_RenderingPropertiesTest, BidirectionalConversion)
{
  // Start with an XCAFDoc_VisMaterialCommon
  XCAFDoc_VisMaterialCommon aOriginalMaterial = CreateMaterial();

  // Convert to rendering properties
  STEPConstruct_RenderingProperties aProps(aOriginalMaterial);

  // Convert back to material
  XCAFDoc_VisMaterialCommon aConvertedMaterial = aProps.CreateXCAFMaterial();

  // Verify that the converted material matches the original
  EXPECT_TRUE(aConvertedMaterial.IsDefined);

  EXPECT_TRUE(AreColorsEqual(aConvertedMaterial.DiffuseColor, aOriginalMaterial.DiffuseColor));
  EXPECT_NEAR(aConvertedMaterial.Transparency, aOriginalMaterial.Transparency, 0.001);
  EXPECT_TRUE(
    AreColorsEqual(aConvertedMaterial.AmbientColor, aOriginalMaterial.AmbientColor, 0.05));
  EXPECT_TRUE(
    AreColorsEqual(aConvertedMaterial.SpecularColor, aOriginalMaterial.SpecularColor, 0.05));
  EXPECT_NEAR(aConvertedMaterial.Shininess, aOriginalMaterial.Shininess, 0.05);
}

// Test Init method with RGBA color
TEST_F(STEPConstruct_RenderingPropertiesTest, InitWithRGBAColor)
{
  STEPConstruct_RenderingProperties aProps;

  // Create an RGBA color with alpha = 0.6 (transparency = 0.4)
  Quantity_ColorRGBA aRgba(Quantity_Color(0.3, 0.6, 0.9, Quantity_TOC_RGB), 0.6f);

  aProps.Init(aRgba);

  EXPECT_TRUE(aProps.IsDefined());
  EXPECT_FALSE(aProps.IsAmbientReflectanceDefined());
  EXPECT_FALSE(aProps.IsDiffuseReflectanceDefined());
  EXPECT_FALSE(aProps.IsSpecularReflectanceDefined());

  Quantity_Color expectedColor(0.3, 0.6, 0.9, Quantity_TOC_RGB);
  EXPECT_TRUE(AreColorsEqual(aProps.SurfaceColor(), expectedColor));
  EXPECT_NEAR(aProps.Transparency(), 0.4, 0.001);
}

// Test Init method with custom rendering method
TEST_F(STEPConstruct_RenderingPropertiesTest, InitWithCustomRenderingMethod)
{
  STEPConstruct_RenderingProperties aProps;

  // Initialize with phong shading
  aProps.Init(mySurfaceColor, myTransparency);
  aProps.SetRenderingMethod(StepVisual_ssmDotShading);

  EXPECT_TRUE(aProps.IsDefined());
  EXPECT_TRUE(AreColorsEqual(aProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aProps.Transparency(), myTransparency, 0.001);
  EXPECT_EQ(aProps.RenderingMethod(), StepVisual_ssmDotShading);
}

// Test the IsMaterialConvertible method
TEST_F(STEPConstruct_RenderingPropertiesTest, MaterialConvertible)
{
  STEPConstruct_RenderingProperties aProps;

  // Initially shouldn't be convertible
  EXPECT_FALSE(aProps.IsMaterialConvertible());

  // After setting basic properties, still shouldn't be convertible
  aProps.Init(mySurfaceColor, myTransparency);
  EXPECT_FALSE(aProps.IsMaterialConvertible());

  // After setting all required properties, should be convertible
  aProps.SetAmbientDiffuseAndSpecularReflectance(myAmbientFactor,
                                                 myDiffuseFactor,
                                                 mySpecularFactor,
                                                 mySpecularExponent,
                                                 mySpecularColor);

  EXPECT_TRUE(aProps.IsMaterialConvertible());
}

// Test with null inputs
TEST_F(STEPConstruct_RenderingPropertiesTest, NullInputs)
{
  STEPConstruct_RenderingProperties aProps;

  // Creating from null STEP rendering properties
  Handle(StepVisual_SurfaceStyleRenderingWithProperties) nullProps;
  aProps.Init(nullProps);

  EXPECT_FALSE(aProps.IsDefined());

  // Creating from null XCAF material
  XCAFDoc_VisMaterialCommon nullMaterial;
  nullMaterial.IsDefined = Standard_False;

  STEPConstruct_RenderingProperties propsFromNull(nullMaterial);
  EXPECT_FALSE(propsFromNull.IsDefined());
}

// Test creating STEP rendering properties with ambient only
TEST_F(STEPConstruct_RenderingPropertiesTest, CreateAmbientOnlyProperties)
{
  STEPConstruct_RenderingProperties aProps;
  aProps.Init(mySurfaceColor, myTransparency);
  aProps.SetAmbientReflectance(myAmbientFactor);

  Handle(StepVisual_SurfaceStyleRenderingWithProperties) aStepProps =
    aProps.CreateRenderingProperties();

  ASSERT_FALSE(aStepProps.IsNull());

  // Verify properties through re-parsing
  STEPConstruct_RenderingProperties aParsedProps(aStepProps);

  EXPECT_TRUE(aParsedProps.IsDefined());
  EXPECT_TRUE(aParsedProps.IsAmbientReflectanceDefined());
  EXPECT_FALSE(aParsedProps.IsDiffuseReflectanceDefined());
  EXPECT_FALSE(aParsedProps.IsSpecularReflectanceDefined());

  EXPECT_TRUE(AreColorsEqual(aParsedProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aParsedProps.Transparency(), myTransparency, 0.001);
  EXPECT_NEAR(aParsedProps.AmbientReflectance(), myAmbientFactor, 0.001);
}

// Test creating STEP rendering properties with ambient and diffuse
TEST_F(STEPConstruct_RenderingPropertiesTest, CreateAmbientAndDiffuseProperties)
{
  STEPConstruct_RenderingProperties aProps;
  aProps.Init(mySurfaceColor, myTransparency);
  aProps.SetAmbientAndDiffuseReflectance(myAmbientFactor, myDiffuseFactor);

  Handle(StepVisual_SurfaceStyleRenderingWithProperties) aStepProps =
    aProps.CreateRenderingProperties();

  ASSERT_FALSE(aStepProps.IsNull());

  // Verify properties through re-parsing
  STEPConstruct_RenderingProperties aParsedProps(aStepProps);

  EXPECT_TRUE(aParsedProps.IsDefined());
  EXPECT_TRUE(aParsedProps.IsAmbientReflectanceDefined());
  EXPECT_TRUE(aParsedProps.IsDiffuseReflectanceDefined());
  EXPECT_FALSE(aParsedProps.IsSpecularReflectanceDefined());

  EXPECT_TRUE(AreColorsEqual(aParsedProps.SurfaceColor(), mySurfaceColor));
  EXPECT_NEAR(aParsedProps.Transparency(), myTransparency, 0.001);
  EXPECT_NEAR(aParsedProps.AmbientReflectance(), myAmbientFactor, 0.001);
  EXPECT_NEAR(aParsedProps.DiffuseReflectance(), myDiffuseFactor, 0.001);
}

// Test handling of non-standard specular color
TEST_F(STEPConstruct_RenderingPropertiesTest, NonStandardSpecularColor)
{
  // Create a material with custom specular color not matching diffuse
  XCAFDoc_VisMaterialCommon aMaterial = CreateMaterial();
  Quantity_Color aCustomSpecular(0.1, 0.8, 0.2, Quantity_TOC_RGB); // Very different from diffuse
  aMaterial.SpecularColor = aCustomSpecular;

  // Convert to rendering properties
  STEPConstruct_RenderingProperties aProps(aMaterial);

  // Verify specular color is preserved as actual color, not just a factor
  EXPECT_TRUE(aProps.IsSpecularColourDefined());
  EXPECT_TRUE(AreColorsEqual(aProps.SpecularColour(), aCustomSpecular));

  // Verify converting back preserves the special color
  XCAFDoc_VisMaterialCommon aReconvertedMaterial = aProps.CreateXCAFMaterial();
  EXPECT_TRUE(AreColorsEqual(aReconvertedMaterial.SpecularColor, aCustomSpecular, 0.05));
}

// Test extreme values for properties
TEST_F(STEPConstruct_RenderingPropertiesTest, ExtremeValues)
{
  STEPConstruct_RenderingProperties aProps;

  // Test with full transparency
  aProps.Init(mySurfaceColor, 1.0);
  EXPECT_NEAR(aProps.Transparency(), 1.0, 0.001);

  // Test with extreme reflectance values
  aProps.SetAmbientDiffuseAndSpecularReflectance(0.0, 2.0, 1.0, 256.0, mySpecularColor);

  // Values should be clamped when creating material
  XCAFDoc_VisMaterialCommon aMaterial = aProps.CreateXCAFMaterial();

  // Ambient should be 0
  EXPECT_NEAR(aMaterial.AmbientColor.Red(), 0.0, 0.001);
  EXPECT_NEAR(aMaterial.AmbientColor.Green(), 0.0, 0.001);
  EXPECT_NEAR(aMaterial.AmbientColor.Blue(), 0.0, 0.001);

  // Shininess should be reasonable value (256/128 = 2, clamped to 1.0)
  EXPECT_NEAR(aMaterial.Shininess, 1.0, 0.1);
}
