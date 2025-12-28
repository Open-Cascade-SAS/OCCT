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
#include <StepVisual_SurfaceStyleElementSelect.hxx>
#include <StepVisual_SurfaceStyleTransparent.hxx>
#include <StepVisual_RenderingPropertiesSelect.hxx>
#include <StepVisual_RenderingPropertiesSelect.hxx>
#include <NCollection_Array1.hxx>
#include <NCollection_HArray1.hxx>
#include <StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular.hxx>
#include <StepVisual_ColourRgb.hxx>
#include <StepVisual_DraughtingPreDefinedColour.hxx>
#include <StepVisual_PreDefinedItem.hxx>
#include <TCollection_HAsciiString.hxx>
#include <XCAFDoc_VisMaterialCommon.hxx>
#include <XCAFDoc_VisMaterial.hxx>

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties()
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const occ::handle<StepVisual_SurfaceStyleRenderingWithProperties>& theRenderingProperties)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
  Init(theRenderingProperties);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Quantity_ColorRGBA& theRGBAColor)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
  Init(theRGBAColor);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const occ::handle<StepVisual_Colour>& theColor,
  const double              theTransparency)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
  Init(theColor, theTransparency);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const XCAFDoc_VisMaterialCommon& theMaterial)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
  Init(theMaterial);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const occ::handle<XCAFDoc_VisMaterial>& theMaterial)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
  Init(theMaterial);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Quantity_Color& theSurfaceColor,
  const double   theTransparency)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(false),
      myAmbientReflectance(0.0, false),
      myDiffuseReflectance(0.0, false),
      mySpecularReflectance(0.0, false),
      mySpecularExponent(0.0, false),
      mySpecularColour(Quantity_NOC_WHITE, false)
{
  Init(theSurfaceColor, theTransparency);
}

//=================================================================================================

void STEPConstruct_RenderingProperties::SetAmbientReflectance(
  const double theAmbientReflectance)
{
  myAmbientReflectance.first  = theAmbientReflectance;
  myAmbientReflectance.second = true;
}

//=================================================================================================

void STEPConstruct_RenderingProperties::SetAmbientAndDiffuseReflectance(
  const double theAmbientReflectance,
  const double theDiffuseReflectance)
{
  myAmbientReflectance.first  = theAmbientReflectance;
  myAmbientReflectance.second = true;
  myDiffuseReflectance.first  = theDiffuseReflectance;
  myDiffuseReflectance.second = true;
}

//=================================================================================================

void STEPConstruct_RenderingProperties::SetAmbientDiffuseAndSpecularReflectance(
  const double   theAmbientReflectance,
  const double   theDiffuseReflectance,
  const double   theSpecularReflectance,
  const double   theSpecularExponent,
  const Quantity_Color& theSpecularColour)
{
  myAmbientReflectance.first   = theAmbientReflectance;
  myAmbientReflectance.second  = true;
  myDiffuseReflectance.first   = theDiffuseReflectance;
  myDiffuseReflectance.second  = true;
  mySpecularReflectance.first  = theSpecularReflectance;
  mySpecularReflectance.second = true;
  mySpecularExponent.first     = theSpecularExponent;
  mySpecularExponent.second    = true;
  mySpecularColour.first       = theSpecularColour;
  mySpecularColour.second      = true;
}

//=================================================================================================

occ::handle<StepVisual_SurfaceStyleRenderingWithProperties> STEPConstruct_RenderingProperties::
  CreateRenderingProperties() const
{
  return CreateRenderingProperties(STEPConstruct_Styles::EncodeColor(mySurfaceColor));
}

//=================================================================================================

occ::handle<StepVisual_SurfaceStyleRenderingWithProperties> STEPConstruct_RenderingProperties::
  CreateRenderingProperties(const occ::handle<StepVisual_Colour>& theRenderColour) const
{
  if (!myIsDefined)
  {
    return nullptr;
  }

  // Create STEP RGB color or use predefined color using STEPConstruct_Styles utility
  occ::handle<StepVisual_Colour> aStepColor =
    !theRenderColour.IsNull() ? theRenderColour : STEPConstruct_Styles::EncodeColor(mySurfaceColor);

  // Count and determine which properties to create.
  // Transparency is always included.
  int aNbProps = 1;

  // Determine which reflectance properties to include
  const bool hasFullReflectance =
    (mySpecularColour.second && mySpecularExponent.second && mySpecularReflectance.second
     && myDiffuseReflectance.second && myAmbientReflectance.second);
  const bool hasDiffuseAndAmbient =
    (myDiffuseReflectance.second && myAmbientReflectance.second && !mySpecularReflectance.second);
  const bool hasAmbientOnly =
    (myAmbientReflectance.second && !myDiffuseReflectance.second && !mySpecularReflectance.second);

  // Add reflectance property if we have any defined
  if (hasFullReflectance || hasDiffuseAndAmbient || hasAmbientOnly)
  {
    aNbProps++;
  }

  // Create properties array
  occ::handle<NCollection_HArray1<StepVisual_RenderingPropertiesSelect>> aProps =
    new NCollection_HArray1<StepVisual_RenderingPropertiesSelect>(1, aNbProps);
  int aPropIndex = 1;

  // Add transparency
  occ::handle<StepVisual_SurfaceStyleTransparent> aTransparent = new StepVisual_SurfaceStyleTransparent;
  aTransparent->Init(myTransparency);
  aProps->ChangeValue(aPropIndex++).SetValue(aTransparent);

  // Add the appropriate reflectance model based on what we have
  if (hasAmbientOnly)
  {
    // Add ambient reflectance only
    occ::handle<StepVisual_SurfaceStyleReflectanceAmbient> aAmbient =
      new StepVisual_SurfaceStyleReflectanceAmbient;
    aAmbient->Init(myAmbientReflectance.first);

    aProps->ChangeValue(aPropIndex++).SetValue(aAmbient);
  }
  else if (hasDiffuseAndAmbient)
  {
    // Add ambient and diffuse reflectance
    occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuse> aAmbientDiffuse =
      new StepVisual_SurfaceStyleReflectanceAmbientDiffuse;
    aAmbientDiffuse->Init(myAmbientReflectance.first, myDiffuseReflectance.first);

    aProps->ChangeValue(aPropIndex++).SetValue(aAmbientDiffuse);
  }
  else if (hasFullReflectance)
  {
    // Add full PBR reflectance model (ambient, diffuse, specular)
    occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular> aFullRefl =
      new StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular;

    aFullRefl->Init(myAmbientReflectance.first,
                    myDiffuseReflectance.first,
                    mySpecularReflectance.first,
                    mySpecularExponent.first,
                    STEPConstruct_Styles::EncodeColor(mySpecularColour.first));

    aProps->ChangeValue(aPropIndex++).SetValue(aFullRefl);
  }

  // Create STEP surface style rendering with properties
  occ::handle<StepVisual_SurfaceStyleRenderingWithProperties> aSSRWP =
    new StepVisual_SurfaceStyleRenderingWithProperties;
  aSSRWP->Init(myRenderingMethod, aStepColor, aProps);
  return aSSRWP;
}

//=================================================================================================

XCAFDoc_VisMaterialCommon STEPConstruct_RenderingProperties::CreateXCAFMaterial() const
{
  if (!myIsDefined)
  {
    XCAFDoc_VisMaterialCommon aMaterial;
    aMaterial.IsDefined = false;
    return aMaterial;
  }

  XCAFDoc_VisMaterialCommon aMaterial;

  // Use surface color as the base diffuse color
  aMaterial.DiffuseColor = mySurfaceColor;
  aMaterial.Transparency = static_cast<float>(myTransparency);
  aMaterial.IsDefined    = true;

  // Set default values for other properties
  aMaterial.AmbientColor  = Quantity_Color(0.1, 0.1, 0.1, Quantity_TOC_RGB);
  aMaterial.SpecularColor = Quantity_Color(0.2, 0.2, 0.2, Quantity_TOC_RGB);
  aMaterial.EmissiveColor = Quantity_Color(0.0, 0.0, 0.0, Quantity_TOC_RGB);
  aMaterial.Shininess     = 1.0f;

  // Handle ambient reflectance - apply factor to diffuse color
  if (myAmbientReflectance.second)
  {
    // Get the reflectance factor, clamped to valid range
    const double aAmbientFactor = std::max(0.0, std::min(1.0, myAmbientReflectance.first));

    // Apply factor to surface color (RGB components individually)
    const double aRed   = mySurfaceColor.Red() * aAmbientFactor;
    const double aGreen = mySurfaceColor.Green() * aAmbientFactor;
    const double aBlue  = mySurfaceColor.Blue() * aAmbientFactor;

    Quantity_Color aAmbientColor(aRed, aGreen, aBlue, Quantity_TOC_RGB);
    aMaterial.AmbientColor = aAmbientColor;
  }

  // Handle specular properties - apply factor to diffuse color or use explicit color
  if (mySpecularColour.second)
  {
    // Use explicitly defined specular color
    aMaterial.SpecularColor = mySpecularColour.first;
  }
  else if (mySpecularReflectance.second)
  {
    // Apply specular reflectance factor to surface color
    const double aSpecularFactor = std::max(0.0, std::min(1.0, mySpecularReflectance.first));

    const double aRed   = mySurfaceColor.Red() * aSpecularFactor;
    const double aGreen = mySurfaceColor.Green() * aSpecularFactor;
    const double aBlue  = mySurfaceColor.Blue() * aSpecularFactor;

    Quantity_Color aSpecularColor(aRed, aGreen, aBlue, Quantity_TOC_RGB);
    aMaterial.SpecularColor = aSpecularColor;
  }

  // Handle shininess (specular exponent)
  if (mySpecularExponent.second)
  {
    // Convert STEP specular exponent to XCAF shininess using fixed scale factor
    const double kScaleFactor = 128.0;
    const double aShininess   = mySpecularExponent.first / kScaleFactor;
    aMaterial.Shininess              = (float)std::min(1.0, aShininess);
  }

  return aMaterial;
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(
  const occ::handle<StepVisual_SurfaceStyleRenderingWithProperties>& theRenderingProperties)
{
  mySurfaceColor        = Quantity_NOC_WHITE;
  myTransparency        = 0.0;
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = false;
  myAmbientReflectance  = std::make_pair(0.0, false);
  myDiffuseReflectance  = std::make_pair(0.0, false);
  mySpecularReflectance = std::make_pair(0.0, false);
  mySpecularExponent    = std::make_pair(0.0, false);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, false);

  if (theRenderingProperties.IsNull())
  {
    return;
  }

  myRenderingMethod                = theRenderingProperties->RenderingMethod();
  occ::handle<StepVisual_Colour> aColor = theRenderingProperties->SurfaceColour();

  // Decode surface color using STEPConstruct_Styles utility
  if (!aColor.IsNull())
  {
    Quantity_Color aDecodedColor;
    if (STEPConstruct_Styles::DecodeColor(aColor, aDecodedColor))
    {
      mySurfaceColor = aDecodedColor;
      myIsDefined    = true;
    }
  }

  // Process rendering properties
  occ::handle<NCollection_HArray1<StepVisual_RenderingPropertiesSelect>> aProperties =
    theRenderingProperties->Properties();
  if (!aProperties.IsNull())
  {
    for (int i = 1; i <= aProperties->Length(); i++)
    {
      const StepVisual_RenderingPropertiesSelect& aPropSelect = aProperties->Value(i);

      // Check for transparency
      occ::handle<StepVisual_SurfaceStyleTransparent> aTransparent =
        aPropSelect.SurfaceStyleTransparent();
      if (!aTransparent.IsNull())
      {
        myTransparency = aTransparent->Transparency();
        myIsDefined    = true;
      }

      // Check for ambient reflectance
      occ::handle<StepVisual_SurfaceStyleReflectanceAmbient> aAmbient =
        aPropSelect.SurfaceStyleReflectanceAmbient();
      occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuse> aAmbientDiffuse =
        occ::down_cast<StepVisual_SurfaceStyleReflectanceAmbientDiffuse>(aAmbient);
      occ::handle<StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular> aFullRefl =
        occ::down_cast<StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular>(aAmbient);
      if (!aFullRefl.IsNull())
      {
        myIsDefined                  = true;
        myAmbientReflectance.first   = aFullRefl->AmbientReflectance();
        myDiffuseReflectance.first   = aFullRefl->DiffuseReflectance();
        mySpecularReflectance.first  = aFullRefl->SpecularReflectance();
        mySpecularExponent.first     = aFullRefl->SpecularExponent();
        myAmbientReflectance.second  = true;
        myDiffuseReflectance.second  = true;
        mySpecularReflectance.second = true;
        mySpecularExponent.second    = true;
        mySpecularColour.second =
          STEPConstruct_Styles::DecodeColor(aFullRefl->SpecularColour(), mySpecularColour.first);
      }
      else if (!aAmbientDiffuse.IsNull())
      {
        myAmbientReflectance.first  = aAmbientDiffuse->AmbientReflectance();
        myAmbientReflectance.second = true;
        myDiffuseReflectance.first  = aAmbientDiffuse->DiffuseReflectance();
        myDiffuseReflectance.second = true;
        myIsDefined                 = true;
      }
      else if (!aAmbient.IsNull())
      {
        myAmbientReflectance.first  = aAmbient->AmbientReflectance();
        myAmbientReflectance.second = true;
        myIsDefined                 = true;
      }
    }
  }
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const Quantity_ColorRGBA& theRGBAColor)
{
  mySurfaceColor        = theRGBAColor.GetRGB();
  myTransparency        = 1.0 - theRGBAColor.Alpha();
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = true;
  myAmbientReflectance  = std::make_pair(0.0, false);
  myDiffuseReflectance  = std::make_pair(0.0, false);
  mySpecularReflectance = std::make_pair(0.0, false);
  mySpecularExponent    = std::make_pair(0.0, false);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, false);
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const occ::handle<StepVisual_Colour>& theColor,
                                             const double              theTransparency)
{
  mySurfaceColor        = Quantity_NOC_WHITE;
  myTransparency        = theTransparency;
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = false;
  myAmbientReflectance  = std::make_pair(0.0, false);
  myDiffuseReflectance  = std::make_pair(0.0, false);
  mySpecularReflectance = std::make_pair(0.0, false);
  mySpecularExponent    = std::make_pair(0.0, false);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, false);

  if (theColor.IsNull())
  {
    return;
  }

  // Decode color using STEPConstruct_Styles utility
  Quantity_Color aDecodedColor;
  if (STEPConstruct_Styles::DecodeColor(theColor, aDecodedColor))
  {
    mySurfaceColor = aDecodedColor;
    myIsDefined    = true;
  }
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const XCAFDoc_VisMaterialCommon& theMaterial)
{
  if (!theMaterial.IsDefined)
  {
    mySurfaceColor = Quantity_NOC_WHITE;
    myTransparency = 0.0;
    myIsDefined    = false;
    return;
  }

  // Basic properties - use diffuse color as the base surface color
  mySurfaceColor    = theMaterial.DiffuseColor;
  myTransparency    = theMaterial.Transparency;
  myRenderingMethod = StepVisual_ssmNormalShading;
  myIsDefined       = true;

  // Reset reflectance properties
  myAmbientReflectance  = std::make_pair(0.0, false);
  myDiffuseReflectance  = std::make_pair(0.0, false);
  mySpecularReflectance = std::make_pair(0.0, false);
  mySpecularExponent    = std::make_pair(0.0, false);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, false);

  // Extract RGB components from diffuse color
  const double aDiffRed   = theMaterial.DiffuseColor.Red();
  const double aDiffGreen = theMaterial.DiffuseColor.Green();
  const double aDiffBlue  = theMaterial.DiffuseColor.Blue();

  // Find maximum diffuse component to avoid division by zero for dark colors
  const double aDiffMax = std::max(aDiffRed, std::max(aDiffGreen, aDiffBlue));

  // Check if ambient color is non-default and diffuse color has non-zero components
  if (aDiffMax > Precision::Confusion())
  {
    // Extract RGB components from ambient color
    double aAmbRed   = theMaterial.AmbientColor.Red();
    double aAmbGreen = theMaterial.AmbientColor.Green();
    double aAmbBlue  = theMaterial.AmbientColor.Blue();

    // Calculate per-channel ratios
    const double aRed = (aDiffRed > Precision::Confusion()) ? aAmbRed / aDiffRed : 0.0;
    const double aGreen =
      (aDiffGreen > Precision::Confusion()) ? aAmbGreen / aDiffGreen : 0.0;
    const double aBlue = (aDiffBlue > Precision::Confusion()) ? aAmbBlue / aDiffBlue : 0.0;

    // Calculate min and max of RGB ratios
    const double aMin = std::min(aRed, std::min(aGreen, aBlue));
    const double aMax = std::max(aRed, std::max(aGreen, aBlue));

    // If ratios are reasonably close, use average as ambient reflectance factor
    // otherwise the ambient color isn't a simple multiplier of diffuse
    const double kMaxRatioDeviation = 0.2; // Max allowed deviation between RGB ratios
    if ((aMax - aMin) < kMaxRatioDeviation)
    {
      // Use average of RGB ratios as the reflectance factor
      double aAmbientFactor = (aRed + aGreen + aBlue) / 3.0;

      // Check if factor is significantly different from default (0.1)
      if (std::abs(aAmbientFactor - 0.1) > 0.01)
      {
        // Clamp to valid range
        aAmbientFactor = std::max(0.0, std::min(1.0, aAmbientFactor));

        myAmbientReflectance.first  = aAmbientFactor;
        myAmbientReflectance.second = true;
      }
    }
  }

  // Diffuse reflectance is always 1.0 for the main color
  myDiffuseReflectance.first  = 1.0;
  myDiffuseReflectance.second = true;

  // Calculate specular reflectance factor relative to diffuse color
  // Compare specular color to diffuse color directly to get reflectance factor
  if (aDiffMax > Precision::Confusion())
  {
    // Extract RGB components from specular color
    const double aSpecRed   = theMaterial.SpecularColor.Red();
    const double aSpecGreen = theMaterial.SpecularColor.Green();
    const double aSpecBlue  = theMaterial.SpecularColor.Blue();

    // Calculate per-channel ratios
    const double aRed = (aDiffRed > Precision::Confusion()) ? aSpecRed / aDiffRed : 0.0;
    const double aGreen =
      (aDiffGreen > Precision::Confusion()) ? aSpecGreen / aDiffGreen : 0.0;
    const double aBlue = (aDiffBlue > Precision::Confusion()) ? aSpecBlue / aDiffBlue : 0.0;

    // Calculate min and max of RGB ratios
    const double aMin = std::min(aRed, std::min(aGreen, aBlue));
    const double aMax = std::max(aRed, std::max(aGreen, aBlue));

    // If ratios are reasonably close, use average as specular reflectance factor
    const double kMaxRatioDeviation = 0.2; // Max allowed deviation between RGB ratios
    if ((aMax - aMin) < kMaxRatioDeviation)
    {
      // Use average of RGB ratios as the reflectance factor
      double aSpecularFactor = (aRed + aGreen + aBlue) / 3.0;

      // Check if factor is significantly different from default (0.2)
      if (std::abs(aSpecularFactor - 0.2) > 0.01)
      {
        // Clamp to valid range
        aSpecularFactor = std::max(0.0, std::min(1.0, aSpecularFactor));

        mySpecularReflectance.first  = aSpecularFactor;
        mySpecularReflectance.second = true;
      }
    }
    else
    {
      // Ratios differ significantly, specular color isn't a simple multiplier of diffuse
      // Store actual specular color
      mySpecularColour.first  = theMaterial.SpecularColor;
      mySpecularColour.second = true;

      // Still compute an average reflectance factor for formats that don't support color
      double aSpecularFactor = (aSpecRed + aSpecGreen + aSpecBlue) / 3.0;
      mySpecularReflectance.first   = aSpecularFactor;
      mySpecularReflectance.second  = true;
    }
  }

  // Convert shininess to specular exponent using fixed scale factor
  if (theMaterial.Shininess >= 0.0f && std::abs(theMaterial.Shininess - 1.0f) > 0.01f)
  {
    const double kScaleFactor = 128.0;
    mySpecularExponent.first         = theMaterial.Shininess * kScaleFactor;
    mySpecularExponent.second        = true;
  }
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const occ::handle<XCAFDoc_VisMaterial>& theMaterial)
{
  if (theMaterial.IsNull())
  {
    return;
  }

  // Use the material to initialize rendering properties
  Init(theMaterial->ConvertToCommonMaterial());
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const Quantity_Color& theSurfaceColor,
                                             const double   theTransparency)
{
  mySurfaceColor        = theSurfaceColor;
  myTransparency        = theTransparency;
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = true;
  myAmbientReflectance  = std::make_pair(0.0, false);
  myDiffuseReflectance  = std::make_pair(0.0, false);
  mySpecularReflectance = std::make_pair(0.0, false);
  mySpecularExponent    = std::make_pair(0.0, false);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, false);
}

//=================================================================================================

bool STEPConstruct_RenderingProperties::IsMaterialConvertible() const
{
  return myIsDefined && myAmbientReflectance.second && myDiffuseReflectance.second
         && mySpecularReflectance.second && mySpecularExponent.second && mySpecularColour.second;
}

//=================================================================================================

double STEPConstruct_RenderingProperties::AmbientReflectance() const
{
  return myAmbientReflectance.first;
}

//=================================================================================================

bool STEPConstruct_RenderingProperties::IsAmbientReflectanceDefined() const
{
  return myAmbientReflectance.second;
}

//=================================================================================================

double STEPConstruct_RenderingProperties::DiffuseReflectance() const
{
  return myDiffuseReflectance.first;
}

//=================================================================================================

bool STEPConstruct_RenderingProperties::IsDiffuseReflectanceDefined() const
{
  return myDiffuseReflectance.second;
}

//=================================================================================================

double STEPConstruct_RenderingProperties::SpecularReflectance() const
{
  return mySpecularReflectance.first;
}

//=================================================================================================

bool STEPConstruct_RenderingProperties::IsSpecularReflectanceDefined() const
{
  return mySpecularReflectance.second;
}

//=================================================================================================

double STEPConstruct_RenderingProperties::SpecularExponent() const
{
  return mySpecularExponent.first;
}

//=================================================================================================

bool STEPConstruct_RenderingProperties::IsSpecularExponentDefined() const
{
  return mySpecularExponent.second;
}

//=================================================================================================

Quantity_Color STEPConstruct_RenderingProperties::SpecularColour() const
{
  return mySpecularColour.first;
}

//=================================================================================================

bool STEPConstruct_RenderingProperties::IsSpecularColourDefined() const
{
  return mySpecularColour.second;
}
