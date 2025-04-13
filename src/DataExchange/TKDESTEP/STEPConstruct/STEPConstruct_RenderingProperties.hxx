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

#ifndef STEPConstruct_RenderingProperties_HeaderFile
#define STEPConstruct_RenderingProperties_HeaderFile

#include <Quantity_Color.hxx>
#include <Quantity_ColorRGBA.hxx>
#include <StepVisual_ShadingSurfaceMethod.hxx>
#include <StepVisual_SurfaceStyleRenderingWithProperties.hxx>
#include <XCAFDoc_VisMaterialCommon.hxx>

#include <utility>

class StepVisual_SurfaceStyleRenderingWithProperties;
class StepVisual_Colour;
class XCAFDoc_VisMaterial;

//! Class for working with STEP rendering properties.
//! Provides functionality to create and manipulate rendering properties
//! used for specifying visual appearance in STEP format.
//! This class handles both parsing of STEP entities and creation of new ones.
class STEPConstruct_RenderingProperties
{
public:
  DEFINE_STANDARD_ALLOC
public:
  //! Default constructor creating an empty rendering properties object
  Standard_EXPORT STEPConstruct_RenderingProperties();

  //! Constructor from STEP rendering properties entity.
  //! Extracts color, transparency, and other properties from the STEP entity.
  //! @param[in] theRenderingProperties rendering properties entity
  Standard_EXPORT STEPConstruct_RenderingProperties(
    const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& theRenderingProperties);

  //! Constructor from RGBA color.
  //! Creates rendering properties with the given color and transparency.
  //! @param[in] theRGBAColor color with transparency
  Standard_EXPORT STEPConstruct_RenderingProperties(const Quantity_ColorRGBA& theRGBAColor);

  //! Constructor from STEP color and transparency value.
  //! Creates rendering properties with the given color and transparency.
  //! @param[in] theColor color
  //! @param[in] theTransparency transparency value
  Standard_EXPORT STEPConstruct_RenderingProperties(const Handle(StepVisual_Colour)& theColor,
                                                    const Standard_Real theTransparency);

  //! Constructor from XCAFDoc_VisMaterialCommon.
  //! Creates rendering properties using material properties from the OCCT material.
  //! @param[in] theMaterial common visualization material properties
  Standard_EXPORT STEPConstruct_RenderingProperties(const XCAFDoc_VisMaterialCommon& theMaterial);

  //! Constructor from XCAFDoc_VisMaterial.
  //! Creates rendering properties using material properties from the OCCT material.
  //! @param[in] theMaterial visualization material properties
  Standard_EXPORT STEPConstruct_RenderingProperties(const Handle(XCAFDoc_VisMaterial)& theMaterial);

  //! Constructor from surface color, transparency, and rendering method.
  //! @param[in] theSurfaceColor surface color
  //! @param[in] theTransparency transparency value
  Standard_EXPORT STEPConstruct_RenderingProperties(const Quantity_Color& theSurfaceColor,
                                                    const Standard_Real   theTransparency = 0.0);

  //! Initializes from STEP rendering properties entity.
  //! Extracts color, transparency, and other properties from the STEP entity.
  //! @param[in] theRenderingProperties rendering properties entity
  Standard_EXPORT void Init(
    const Handle(StepVisual_SurfaceStyleRenderingWithProperties)& theRenderingProperties);

  //! Initializes from RGBA color.
  //! @param[in] theRGBAColor color with transparency
  Standard_EXPORT void Init(const Quantity_ColorRGBA& theRGBAColor);

  //! Initializes from STEP color and transparency value.
  //! @param[in] theColor STEP color entity
  //! @param[in] theTransparency transparency value
  Standard_EXPORT void Init(const Handle(StepVisual_Colour)& theColor,
                            const Standard_Real              theTransparency);

  //! Initializes from XCAFDoc_VisMaterialCommon.
  //! @param[in] theMaterial common visualization material properties
  Standard_EXPORT void Init(const XCAFDoc_VisMaterialCommon& theMaterial);

  //! Initializes from XCAFDoc_VisMaterial.
  //! @param[in] theMaterial visualization material properties
  Standard_EXPORT void Init(const Handle(XCAFDoc_VisMaterial)& theMaterial);

  //! Initializes from surface color, transparency and rendering method.
  //! @param[in] theSurfaceColor surface color
  //! @param[in] theTransparency transparency value
  Standard_EXPORT void Init(const Quantity_Color& theSurfaceColor,
                            const Standard_Real   theTransparency = 0.0);

  //! Sets ambient reflectance value
  //! @param[in] theAmbientReflectance ambient reflectance value
  Standard_EXPORT void SetAmbientReflectance(const Standard_Real theAmbientReflectance);

  //! Sets ambient and diffuse reflectance values
  //! @param[in] theAmbientReflectance ambient reflectance value
  //! @param[in] theDiffuseReflectance diffuse reflectance value
  Standard_EXPORT void SetAmbientAndDiffuseReflectance(const Standard_Real theAmbientReflectance,
                                                       const Standard_Real theDiffuseReflectance);

  //! Sets ambient, diffuse and specular reflectance values
  //! @param[in] theAmbientReflectance ambient reflectance value
  //! @param[in] theDiffuseReflectance diffuse reflectance value
  //! @param[in] theSpecularReflectance specular reflectance value
  //! @param[in] theSpecularExponent specular exponent value
  //! @param[in] theSpecularColour specular color
  Standard_EXPORT void SetAmbientDiffuseAndSpecularReflectance(
    const Standard_Real   theAmbientReflectance,
    const Standard_Real   theDiffuseReflectance,
    const Standard_Real   theSpecularReflectance,
    const Standard_Real   theSpecularExponent,
    const Quantity_Color& theSpecularColour);

  //! Creates and returns rendering properties entity
  //! @return created rendering properties entity
  Standard_EXPORT Handle(StepVisual_SurfaceStyleRenderingWithProperties) CreateRenderingProperties()
    const;

  // Creates and returns rendering properties entity with the specified color
  //! @param[in] theRenderColour color to be used for rendering
  //! @return created rendering properties entity
  Standard_EXPORT Handle(StepVisual_SurfaceStyleRenderingWithProperties) CreateRenderingProperties(
    const Handle(StepVisual_Colour)& theRenderColour) const;

  //! Creates and returns XCAF material entity
  //! @return created XCAF material entity
  Standard_EXPORT XCAFDoc_VisMaterialCommon CreateXCAFMaterial() const;

  //! Creates the ColorRGBA object from the current color and transparency
  //! @return ColorRGBA object
  Quantity_ColorRGBA GetRGBAColor() const
  {
    return Quantity_ColorRGBA(mySurfaceColor, static_cast<float>(1.0 - myTransparency));
  }

  //! Returns surface color
  //! @return surface color
  Quantity_Color SurfaceColor() const { return mySurfaceColor; }

  //! Returns transparency value
  //! @return transparency value
  Standard_Real Transparency() const { return myTransparency; }

  //! Returns rendering method
  //! @return rendering method
  StepVisual_ShadingSurfaceMethod RenderingMethod() const { return myRenderingMethod; }

  //! Sets rendering method
  //! @param[in] theRenderingMethod rendering method
  void SetRenderingMethod(const StepVisual_ShadingSurfaceMethod theRenderingMethod)
  {
    myRenderingMethod = theRenderingMethod;
  }

  //! Returns whether the rendering properties are defined
  //! @return true if defined, false otherwise
  Standard_Boolean IsDefined() const { return myIsDefined; }

  //! Returns whether material is convertible to STEP
  //! @return true if fully defined for conversion, false otherwise
  Standard_EXPORT Standard_Boolean IsMaterialConvertible() const;

  //! Returns ambient reflectance value
  //! @return ambient reflectance value
  Standard_EXPORT Standard_Real AmbientReflectance() const;

  //! Returns whether ambient reflectance is defined
  //! @return true if defined, false otherwise
  Standard_EXPORT Standard_Boolean IsAmbientReflectanceDefined() const;

  //! Returns diffuse reflectance value
  //! @return diffuse reflectance value
  Standard_EXPORT Standard_Real DiffuseReflectance() const;

  //! Returns whether diffuse reflectance is defined
  //! @return true if defined, false otherwise
  Standard_EXPORT Standard_Boolean IsDiffuseReflectanceDefined() const;

  //! Returns specular reflectance value
  //! @return specular reflectance value
  Standard_EXPORT Standard_Real SpecularReflectance() const;

  //! Returns whether specular reflectance is defined
  //! @return true if defined, false otherwise
  Standard_EXPORT Standard_Boolean IsSpecularReflectanceDefined() const;

  //! Returns specular exponent value
  //! @return specular exponent value
  Standard_EXPORT Standard_Real SpecularExponent() const;

  //! Returns whether specular exponent is defined
  //! @return true if defined, false otherwise
  Standard_EXPORT Standard_Boolean IsSpecularExponentDefined() const;

  //! Returns specular color
  //! @return specular color
  Standard_EXPORT Quantity_Color SpecularColour() const;

  //! Returns whether specular color is defined
  //! @return true if defined, false otherwise
  Standard_EXPORT Standard_Boolean IsSpecularColourDefined() const;

private:
  Quantity_Color mySurfaceColor; //!< Surface colour used for rendering
  Standard_Real  myTransparency; //!< Transparency value (0.0 - opaque, 1.0 - fully transparent)
  StepVisual_ShadingSurfaceMethod myRenderingMethod; //!< Rendering method used for shading
  Standard_Boolean myIsDefined; //!< Flag indicating if rendering properties are defined

  //! Ambient reflectance value, applyed on the surface color
  std::pair<Standard_Real, Standard_Boolean> myAmbientReflectance;
  //! Diffuse reflectance value, applyed on the surface color
  std::pair<Standard_Real, Standard_Boolean> myDiffuseReflectance;
  //! Specular reflectance value, applyed on the surface color
  std::pair<Standard_Real, Standard_Boolean> mySpecularReflectance;
  //! Specular exponent value, applyed on the surface color
  std::pair<Standard_Real, Standard_Boolean> mySpecularExponent;
  //! Specular color, applyed on the surface color
  std::pair<Quantity_Color, Standard_Boolean> mySpecularColour;
};

#endif // STEPConstruct_RenderingProperties_HeaderFile