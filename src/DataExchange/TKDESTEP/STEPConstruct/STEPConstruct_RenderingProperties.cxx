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
#include <StepVisual_HArray1OfRenderingPropertiesSelect.hxx>
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
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& theRenderingProperties)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
  Init(theRenderingProperties);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Quantity_ColorRGBA& theRGBAColor)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
  Init(theRGBAColor);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Handle(StepVisual_Colour)& theColor,
  const Standard_Real              theTransparency)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
  Init(theColor, theTransparency);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const XCAFDoc_VisMaterialCommon& theMaterial)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
  Init(theMaterial);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Handle(XCAFDoc_VisMaterial)& theMaterial)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
  Init(theMaterial);
}

//=================================================================================================

STEPConstruct_RenderingProperties::STEPConstruct_RenderingProperties(
  const Quantity_Color& theSurfaceColor,
  const Standard_Real   theTransparency)
    : mySurfaceColor(Quantity_NOC_WHITE),
      myTransparency(0.0),
      myRenderingMethod(StepVisual_ssmNormalShading),
      myIsDefined(Standard_False),
      myAmbientReflectance(0.0, Standard_False),
      myDiffuseReflectance(0.0, Standard_False),
      mySpecularReflectance(0.0, Standard_False),
      mySpecularExponent(0.0, Standard_False),
      mySpecularColour(Quantity_NOC_WHITE, Standard_False)
{
  Init(theSurfaceColor, theTransparency);
}

//=================================================================================================

void STEPConstruct_RenderingProperties::SetAmbientReflectance(
  const Standard_Real theAmbientReflectance)
{
  myAmbientReflectance.first  = theAmbientReflectance;
  myAmbientReflectance.second = Standard_True;
}

//=================================================================================================

void STEPConstruct_RenderingProperties::SetAmbientAndDiffuseReflectance(
  const Standard_Real theAmbientReflectance,
  const Standard_Real theDiffuseReflectance)
{
  myAmbientReflectance.first  = theAmbientReflectance;
  myAmbientReflectance.second = Standard_True;
  myDiffuseReflectance.first  = theDiffuseReflectance;
  myDiffuseReflectance.second = Standard_True;
}

//=================================================================================================

void STEPConstruct_RenderingProperties::SetAmbientDiffuseAndSpecularReflectance(
  const Standard_Real   theAmbientReflectance,
  const Standard_Real   theDiffuseReflectance,
  const Standard_Real   theSpecularReflectance,
  const Standard_Real   theSpecularExponent,
  const Quantity_Color& theSpecularColour)
{
  myAmbientReflectance.first   = theAmbientReflectance;
  myAmbientReflectance.second  = Standard_True;
  myDiffuseReflectance.first   = theDiffuseReflectance;
  myDiffuseReflectance.second  = Standard_True;
  mySpecularReflectance.first  = theSpecularReflectance;
  mySpecularReflectance.second = Standard_True;
  mySpecularExponent.first     = theSpecularExponent;
  mySpecularExponent.second    = Standard_True;
  mySpecularColour.first       = theSpecularColour;
  mySpecularColour.second      = Standard_True;
}

//=================================================================================================

Handle(StepVisual_SurfaceStyleRenderingWithProperties) STEPConstruct_RenderingProperties::
  CreateRenderingProperties() const
{
  return CreateRenderingProperties(STEPConstruct_Styles::EncodeColor(mySurfaceColor));
}

//=================================================================================================

Handle(StepVisual_SurfaceStyleRenderingWithProperties) STEPConstruct_RenderingProperties::
  CreateRenderingProperties(const Handle(StepVisual_Colour)& theRenderColour) const
{
  if (!myIsDefined)
  {
    return nullptr;
  }

  // Create STEP RGB color or use predefined color using STEPConstruct_Styles utility
  Handle(StepVisual_Colour) aStepColor =
    !theRenderColour.IsNull() ? theRenderColour : STEPConstruct_Styles::EncodeColor(mySurfaceColor);

  // Count and determine which properties to create.
  // Transparency is always included.
  Standard_Integer aNbProps = 1;

  // Determine which reflectance properties to include
  const Standard_Boolean hasFullReflectance =
    (mySpecularColour.second && mySpecularExponent.second && mySpecularReflectance.second
     && myDiffuseReflectance.second && myAmbientReflectance.second);
  const Standard_Boolean hasDiffuseAndAmbient =
    (myDiffuseReflectance.second && myAmbientReflectance.second && !mySpecularReflectance.second);
  const Standard_Boolean hasAmbientOnly =
    (myAmbientReflectance.second && !myDiffuseReflectance.second && !mySpecularReflectance.second);

  // Add reflectance property if we have any defined
  if (hasFullReflectance || hasDiffuseAndAmbient || hasAmbientOnly)
  {
    aNbProps++;
  }

  // Create properties array
  Handle(StepVisual_HArray1OfRenderingPropertiesSelect) aProps =
    new StepVisual_HArray1OfRenderingPropertiesSelect(1, aNbProps);
  Standard_Integer aPropIndex = 1;

  // Add transparency
  Handle(StepVisual_SurfaceStyleTransparent) aTransparent = new StepVisual_SurfaceStyleTransparent;
  aTransparent->Init(myTransparency);
  aProps->ChangeValue(aPropIndex++).SetValue(aTransparent);

  // Add the appropriate reflectance model based on what we have
  if (hasAmbientOnly)
  {
    // Add ambient reflectance only
    Handle(StepVisual_SurfaceStyleReflectanceAmbient) aAmbient =
      new StepVisual_SurfaceStyleReflectanceAmbient;
    aAmbient->Init(myAmbientReflectance.first);

    aProps->ChangeValue(aPropIndex++).SetValue(aAmbient);
  }
  else if (hasDiffuseAndAmbient)
  {
    // Add ambient and diffuse reflectance
    Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuse) aAmbientDiffuse =
      new StepVisual_SurfaceStyleReflectanceAmbientDiffuse;
    aAmbientDiffuse->Init(myAmbientReflectance.first, myDiffuseReflectance.first);

    aProps->ChangeValue(aPropIndex++).SetValue(aAmbientDiffuse);
  }
  else if (hasFullReflectance)
  {
    // Add full PBR reflectance model (ambient, diffuse, specular)
    Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular) aFullRefl =
      new StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular;

    aFullRefl->Init(myAmbientReflectance.first,
                    myDiffuseReflectance.first,
                    mySpecularReflectance.first,
                    mySpecularExponent.first,
                    STEPConstruct_Styles::EncodeColor(mySpecularColour.first));

    aProps->ChangeValue(aPropIndex++).SetValue(aFullRefl);
  }

  // Create STEP surface style rendering with properties
  Handle(StepVisual_SurfaceStyleRenderingWithProperties) aSSRWP =
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
    aMaterial.IsDefined = Standard_False;
    return aMaterial;
  }

  XCAFDoc_VisMaterialCommon aMaterial;

  // Use surface color as the base diffuse color
  aMaterial.DiffuseColor = mySurfaceColor;
  aMaterial.Transparency = static_cast<float>(myTransparency);
  aMaterial.IsDefined    = Standard_True;

  // Set default values for other properties
  aMaterial.AmbientColor  = Quantity_Color(0.1, 0.1, 0.1, Quantity_TOC_RGB);
  aMaterial.SpecularColor = Quantity_Color(0.2, 0.2, 0.2, Quantity_TOC_RGB);
  aMaterial.EmissiveColor = Quantity_Color(0.0, 0.0, 0.0, Quantity_TOC_RGB);
  aMaterial.Shininess     = 1.0f;

  // Handle ambient reflectance - apply factor to diffuse color
  if (myAmbientReflectance.second)
  {
    // Get the reflectance factor, clamped to valid range
    const Standard_Real aAmbientFactor = std::max(0.0, std::min(1.0, myAmbientReflectance.first));

    // Apply factor to surface color (RGB components individually)
    const Standard_Real aRed   = mySurfaceColor.Red() * aAmbientFactor;
    const Standard_Real aGreen = mySurfaceColor.Green() * aAmbientFactor;
    const Standard_Real aBlue  = mySurfaceColor.Blue() * aAmbientFactor;

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
    const Standard_Real aSpecularFactor = std::max(0.0, std::min(1.0, mySpecularReflectance.first));

    const Standard_Real aRed   = mySurfaceColor.Red() * aSpecularFactor;
    const Standard_Real aGreen = mySurfaceColor.Green() * aSpecularFactor;
    const Standard_Real aBlue  = mySurfaceColor.Blue() * aSpecularFactor;

    Quantity_Color aSpecularColor(aRed, aGreen, aBlue, Quantity_TOC_RGB);
    aMaterial.SpecularColor = aSpecularColor;
  }

  // Handle shininess (specular exponent)
  if (mySpecularExponent.second)
  {
    // Convert STEP specular exponent to XCAF shininess using fixed scale factor
    const Standard_Real kScaleFactor = 128.0;
    const Standard_Real aShininess   = mySpecularExponent.first / kScaleFactor;
    aMaterial.Shininess              = (Standard_ShortReal)std::min(1.0, aShininess);
  }

  return aMaterial;
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(
  const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& theRenderingProperties)
{
  mySurfaceColor        = Quantity_NOC_WHITE;
  myTransparency        = 0.0;
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = Standard_False;
  myAmbientReflectance  = std::make_pair(0.0, Standard_False);
  myDiffuseReflectance  = std::make_pair(0.0, Standard_False);
  mySpecularReflectance = std::make_pair(0.0, Standard_False);
  mySpecularExponent    = std::make_pair(0.0, Standard_False);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, Standard_False);

  if (theRenderingProperties.IsNull())
  {
    return;
  }

  myRenderingMethod                = theRenderingProperties->RenderingMethod();
  Handle(StepVisual_Colour) aColor = theRenderingProperties->SurfaceColour();

  // Decode surface color using STEPConstruct_Styles utility
  if (!aColor.IsNull())
  {
    Quantity_Color aDecodedColor;
    if (STEPConstruct_Styles::DecodeColor(aColor, aDecodedColor))
    {
      mySurfaceColor = aDecodedColor;
      myIsDefined    = Standard_True;
    }
  }

  // Process rendering properties
  Handle(StepVisual_HArray1OfRenderingPropertiesSelect) aProperties =
    theRenderingProperties->Properties();
  if (!aProperties.IsNull())
  {
    for (Standard_Integer i = 1; i <= aProperties->Length(); i++)
    {
      const StepVisual_RenderingPropertiesSelect& aPropSelect = aProperties->Value(i);

      // Check for transparency
      Handle(StepVisual_SurfaceStyleTransparent) aTransparent =
        aPropSelect.SurfaceStyleTransparent();
      if (!aTransparent.IsNull())
      {
        myTransparency = aTransparent->Transparency();
        myIsDefined    = Standard_True;
      }

      // Check for ambient reflectance
      Handle(StepVisual_SurfaceStyleReflectanceAmbient) aAmbient =
        aPropSelect.SurfaceStyleReflectanceAmbient();
      Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuse) aAmbientDiffuse =
        Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuse)::DownCast(aAmbient);
      Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular) aFullRefl =
        Handle(StepVisual_SurfaceStyleReflectanceAmbientDiffuseSpecular)::DownCast(aAmbient);
      if (!aFullRefl.IsNull())
      {
        myIsDefined                  = Standard_True;
        myAmbientReflectance.first   = aFullRefl->AmbientReflectance();
        myDiffuseReflectance.first   = aFullRefl->DiffuseReflectance();
        mySpecularReflectance.first  = aFullRefl->SpecularReflectance();
        mySpecularExponent.first     = aFullRefl->SpecularExponent();
        myAmbientReflectance.second  = Standard_True;
        myDiffuseReflectance.second  = Standard_True;
        mySpecularReflectance.second = Standard_True;
        mySpecularExponent.second    = Standard_True;
        mySpecularColour.second =
          STEPConstruct_Styles::DecodeColor(aFullRefl->SpecularColour(), mySpecularColour.first);
      }
      else if (!aAmbientDiffuse.IsNull())
      {
        myAmbientReflectance.first  = aAmbientDiffuse->AmbientReflectance();
        myAmbientReflectance.second = Standard_True;
        myDiffuseReflectance.first  = aAmbientDiffuse->DiffuseReflectance();
        myDiffuseReflectance.second = Standard_True;
        myIsDefined                 = Standard_True;
      }
      else if (!aAmbient.IsNull())
      {
        myAmbientReflectance.first  = aAmbient->AmbientReflectance();
        myAmbientReflectance.second = Standard_True;
        myIsDefined                 = Standard_True;
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
  myIsDefined           = Standard_True;
  myAmbientReflectance  = std::make_pair(0.0, Standard_False);
  myDiffuseReflectance  = std::make_pair(0.0, Standard_False);
  mySpecularReflectance = std::make_pair(0.0, Standard_False);
  mySpecularExponent    = std::make_pair(0.0, Standard_False);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, Standard_False);
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const Handle(StepVisual_Colour)& theColor,
                                             const Standard_Real              theTransparency)
{
  mySurfaceColor        = Quantity_NOC_WHITE;
  myTransparency        = theTransparency;
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = Standard_False;
  myAmbientReflectance  = std::make_pair(0.0, Standard_False);
  myDiffuseReflectance  = std::make_pair(0.0, Standard_False);
  mySpecularReflectance = std::make_pair(0.0, Standard_False);
  mySpecularExponent    = std::make_pair(0.0, Standard_False);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, Standard_False);

  if (theColor.IsNull())
  {
    return;
  }

  // Decode color using STEPConstruct_Styles utility
  Quantity_Color aDecodedColor;
  if (STEPConstruct_Styles::DecodeColor(theColor, aDecodedColor))
  {
    mySurfaceColor = aDecodedColor;
    myIsDefined    = Standard_True;
  }
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const XCAFDoc_VisMaterialCommon& theMaterial)
{
  if (!theMaterial.IsDefined)
  {
    mySurfaceColor = Quantity_NOC_WHITE;
    myTransparency = 0.0;
    myIsDefined    = Standard_False;
    return;
  }

  // Basic properties - use diffuse color as the base surface color
  mySurfaceColor    = theMaterial.DiffuseColor;
  myTransparency    = theMaterial.Transparency;
  myRenderingMethod = StepVisual_ssmNormalShading;
  myIsDefined       = Standard_True;

  // Reset reflectance properties
  myAmbientReflectance  = std::make_pair(0.0, Standard_False);
  myDiffuseReflectance  = std::make_pair(0.0, Standard_False);
  mySpecularReflectance = std::make_pair(0.0, Standard_False);
  mySpecularExponent    = std::make_pair(0.0, Standard_False);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, Standard_False);

  // Extract RGB components from diffuse color
  const Standard_Real aDiffRed   = theMaterial.DiffuseColor.Red();
  const Standard_Real aDiffGreen = theMaterial.DiffuseColor.Green();
  const Standard_Real aDiffBlue  = theMaterial.DiffuseColor.Blue();

  // Find maximum diffuse component to avoid division by zero for dark colors
  const Standard_Real aDiffMax = std::max(aDiffRed, std::max(aDiffGreen, aDiffBlue));

  // Check if ambient color is non-default and diffuse color has non-zero components
  if (aDiffMax > Precision::Confusion())
  {
    // Extract RGB components from ambient color
    Standard_Real aAmbRed   = theMaterial.AmbientColor.Red();
    Standard_Real aAmbGreen = theMaterial.AmbientColor.Green();
    Standard_Real aAmbBlue  = theMaterial.AmbientColor.Blue();

    // Calculate per-channel ratios
    const Standard_Real aRed = (aDiffRed > Precision::Confusion()) ? aAmbRed / aDiffRed : 0.0;
    const Standard_Real aGreen =
      (aDiffGreen > Precision::Confusion()) ? aAmbGreen / aDiffGreen : 0.0;
    const Standard_Real aBlue = (aDiffBlue > Precision::Confusion()) ? aAmbBlue / aDiffBlue : 0.0;

    // Calculate min and max of RGB ratios
    const Standard_Real aMin = std::min(aRed, std::min(aGreen, aBlue));
    const Standard_Real aMax = std::max(aRed, std::max(aGreen, aBlue));

    // If ratios are reasonably close, use average as ambient reflectance factor
    // otherwise the ambient color isn't a simple multiplier of diffuse
    const Standard_Real kMaxRatioDeviation = 0.2; // Max allowed deviation between RGB ratios
    if ((aMax - aMin) < kMaxRatioDeviation)
    {
      // Use average of RGB ratios as the reflectance factor
      Standard_Real aAmbientFactor = (aRed + aGreen + aBlue) / 3.0;

      // Check if factor is significantly different from default (0.1)
      if (std::abs(aAmbientFactor - 0.1) > 0.01)
      {
        // Clamp to valid range
        aAmbientFactor = std::max(0.0, std::min(1.0, aAmbientFactor));

        myAmbientReflectance.first  = aAmbientFactor;
        myAmbientReflectance.second = Standard_True;
      }
    }
  }

  // Diffuse reflectance is always 1.0 for the main color
  myDiffuseReflectance.first  = 1.0;
  myDiffuseReflectance.second = Standard_True;

  // Calculate specular reflectance factor relative to diffuse color
  // Compare specular color to diffuse color directly to get reflectance factor
  if (aDiffMax > Precision::Confusion())
  {
    // Extract RGB components from specular color
    const Standard_Real aSpecRed   = theMaterial.SpecularColor.Red();
    const Standard_Real aSpecGreen = theMaterial.SpecularColor.Green();
    const Standard_Real aSpecBlue  = theMaterial.SpecularColor.Blue();

    // Calculate per-channel ratios
    const Standard_Real aRed = (aDiffRed > Precision::Confusion()) ? aSpecRed / aDiffRed : 0.0;
    const Standard_Real aGreen =
      (aDiffGreen > Precision::Confusion()) ? aSpecGreen / aDiffGreen : 0.0;
    const Standard_Real aBlue = (aDiffBlue > Precision::Confusion()) ? aSpecBlue / aDiffBlue : 0.0;

    // Calculate min and max of RGB ratios
    const Standard_Real aMin = std::min(aRed, std::min(aGreen, aBlue));
    const Standard_Real aMax = std::max(aRed, std::max(aGreen, aBlue));

    // If ratios are reasonably close, use average as specular reflectance factor
    const Standard_Real kMaxRatioDeviation = 0.2; // Max allowed deviation between RGB ratios
    if ((aMax - aMin) < kMaxRatioDeviation)
    {
      // Use average of RGB ratios as the reflectance factor
      Standard_Real aSpecularFactor = (aRed + aGreen + aBlue) / 3.0;

      // Check if factor is significantly different from default (0.2)
      if (std::abs(aSpecularFactor - 0.2) > 0.01)
      {
        // Clamp to valid range
        aSpecularFactor = std::max(0.0, std::min(1.0, aSpecularFactor));

        mySpecularReflectance.first  = aSpecularFactor;
        mySpecularReflectance.second = Standard_True;
      }
    }
    else
    {
      // Ratios differ significantly, specular color isn't a simple multiplier of diffuse
      // Store actual specular color
      mySpecularColour.first  = theMaterial.SpecularColor;
      mySpecularColour.second = Standard_True;

      // Still compute an average reflectance factor for formats that don't support color
      Standard_Real aSpecularFactor = (aSpecRed + aSpecGreen + aSpecBlue) / 3.0;
      mySpecularReflectance.first   = aSpecularFactor;
      mySpecularReflectance.second  = Standard_True;
    }
  }

  // Convert shininess to specular exponent using fixed scale factor
  if (theMaterial.Shininess >= 0.0f && std::abs(theMaterial.Shininess - 1.0f) > 0.01f)
  {
    const Standard_Real kScaleFactor = 128.0;
    mySpecularExponent.first         = theMaterial.Shininess * kScaleFactor;
    mySpecularExponent.second        = Standard_True;
  }
}

//=================================================================================================

void STEPConstruct_RenderingProperties::Init(const Handle(XCAFDoc_VisMaterial)& theMaterial)
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
                                             const Standard_Real   theTransparency)
{
  mySurfaceColor        = theSurfaceColor;
  myTransparency        = theTransparency;
  myRenderingMethod     = StepVisual_ssmNormalShading;
  myIsDefined           = Standard_True;
  myAmbientReflectance  = std::make_pair(0.0, Standard_False);
  myDiffuseReflectance  = std::make_pair(0.0, Standard_False);
  mySpecularReflectance = std::make_pair(0.0, Standard_False);
  mySpecularExponent    = std::make_pair(0.0, Standard_False);
  mySpecularColour      = std::make_pair(Quantity_NOC_WHITE, Standard_False);
}

//=================================================================================================

Standard_Boolean STEPConstruct_RenderingProperties::IsMaterialConvertible() const
{
  return myIsDefined && myAmbientReflectance.second && myDiffuseReflectance.second
         && mySpecularReflectance.second && mySpecularExponent.second && mySpecularColour.second;
}

//=================================================================================================

Standard_Real STEPConstruct_RenderingProperties::AmbientReflectance() const
{
  return myAmbientReflectance.first;
}

//=================================================================================================

Standard_Boolean STEPConstruct_RenderingProperties::IsAmbientReflectanceDefined() const
{
  return myAmbientReflectance.second;
}

//=================================================================================================

Standard_Real STEPConstruct_RenderingProperties::DiffuseReflectance() const
{
  return myDiffuseReflectance.first;
}

//=================================================================================================

Standard_Boolean STEPConstruct_RenderingProperties::IsDiffuseReflectanceDefined() const
{
  return myDiffuseReflectance.second;
}

//=================================================================================================

Standard_Real STEPConstruct_RenderingProperties::SpecularReflectance() const
{
  return mySpecularReflectance.first;
}

//=================================================================================================

Standard_Boolean STEPConstruct_RenderingProperties::IsSpecularReflectanceDefined() const
{
  return mySpecularReflectance.second;
}

//=================================================================================================

Standard_Real STEPConstruct_RenderingProperties::SpecularExponent() const
{
  return mySpecularExponent.first;
}

//=================================================================================================

Standard_Boolean STEPConstruct_RenderingProperties::IsSpecularExponentDefined() const
{
  return mySpecularExponent.second;
}

//=================================================================================================

Quantity_Color STEPConstruct_RenderingProperties::SpecularColour() const
{
  return mySpecularColour.first;
}

//=================================================================================================

Standard_Boolean STEPConstruct_RenderingProperties::IsSpecularColourDefined() const
{
  return mySpecularColour.second;
}
