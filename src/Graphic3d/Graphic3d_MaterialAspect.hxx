// Created by: NW,JPB,CAL
// Copyright (c) 1991-1999 Matra Datavision
// Copyright (c) 1999-2014 OPEN CASCADE SAS
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

#ifndef _Graphic3d_MaterialAspect_HeaderFile
#define _Graphic3d_MaterialAspect_HeaderFile

#include <Graphic3d_BSDF.hxx>
#include <Graphic3d_PBRMaterial.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <Graphic3d_TypeOfMaterial.hxx>
#include <Graphic3d_TypeOfReflection.hxx>
#include <TCollection_AsciiString.hxx>
#include <Quantity_Color.hxx>

//! This class allows the definition of the type of a surface.
//! Aspect attributes of a 3d face.
//! Keywords: Material, FillArea, Shininess, Ambient, Color, Diffuse,
//! Specular, Transparency, Emissive, ReflectionMode,
//! BackFace, FrontFace, Reflection, Absorption
class Graphic3d_MaterialAspect
{
public:
  DEFINE_STANDARD_ALLOC

  //! Returns the number of predefined textures.
  static Standard_Integer NumberOfMaterials() { return Graphic3d_NameOfMaterial_DEFAULT; }

  //! Returns the name of the predefined material of specified rank within range [1,
  //! NumberOfMaterials()].
  Standard_EXPORT static Standard_CString MaterialName(const Standard_Integer theRank);

  //! Returns the type of the predefined material of specified rank within range [1,
  //! NumberOfMaterials()].
  Standard_EXPORT static Graphic3d_TypeOfMaterial MaterialType(const Standard_Integer theRank);

  //! Finds the material for specified name.
  //! @param[in] theName   name to find
  //! @param[out] theMat   found material
  //! @return FALSE if name was unrecognized
  Standard_EXPORT static Standard_Boolean MaterialFromName(const Standard_CString    theName,
                                                           Graphic3d_NameOfMaterial& theMat);

  //! Returns the material for specified name or Graphic3d_NameOfMaterial_DEFAULT if name is
  //! unknown.
  static Graphic3d_NameOfMaterial MaterialFromName(const Standard_CString theName)
  {
    Graphic3d_NameOfMaterial aMat = Graphic3d_NameOfMaterial_DEFAULT;
    MaterialFromName(theName, aMat);
    return aMat;
  }

public:
  //! Creates a material from default values.
  Standard_EXPORT Graphic3d_MaterialAspect();

  //! Creates a generic material.
  Standard_EXPORT Graphic3d_MaterialAspect(const Graphic3d_NameOfMaterial theName);

  //! Returns the material name (within predefined enumeration).
  Graphic3d_NameOfMaterial Name() const { return myMaterialName; }

  //! Returns the material name within predefined enumeration which has been requested (before
  //! modifications).
  Graphic3d_NameOfMaterial RequestedName() const { return myRequestedMaterialName; }

  //! Returns the given name of this material. This might be:
  //! - given name set by method ::SetMaterialName()
  //! - standard name for a material within enumeration
  //! - "UserDefined" for non-standard material without name specified externally.
  const TCollection_AsciiString& StringName() const { return myStringName; }

  //! Returns the given name of this material. This might be:
  Standard_CString MaterialName() const { return myStringName.ToCString(); }

  //! The current material become a "UserDefined" material.
  //! Set the name of the "UserDefined" material.
  void SetMaterialName(const TCollection_AsciiString& theName)
  {
    // if a component of a "standard" material change, the
    // result is no more standard (a blue gold is not a gold)
    myMaterialName = Graphic3d_NameOfMaterial_UserDefined;
    myStringName   = theName;
  }

  //! Resets the material with the original values according to
  //! the material name but leave the current color values untouched
  //! for the material of type ASPECT.
  void Reset() { init(myRequestedMaterialName); }

  //! Returns the diffuse color of the surface.
  //! WARNING! This method does NOT return color for Graphic3d_MATERIAL_ASPECT material (color is
  //! defined by Graphic3d_Aspects::InteriorColor()).
  const Quantity_Color& Color() const { return myColors[Graphic3d_TOR_DIFFUSE]; }

  //! Modifies the ambient and diffuse color of the surface.
  //! WARNING! Has no effect for Graphic3d_MATERIAL_ASPECT material (color should be set to
  //! Graphic3d_Aspects::SetInteriorColor()).
  Standard_EXPORT void SetColor(const Quantity_Color& theColor);

  //! Returns the transparency coefficient of the surface (1.0 - Alpha); 0.0 means opaque.
  Standard_ShortReal Transparency() const { return myTransparencyCoef; }

  //! Returns the alpha coefficient of the surface (1.0 - Transparency); 1.0 means opaque.
  Standard_ShortReal Alpha() const { return 1.0f - myTransparencyCoef; }

  //! Modifies the transparency coefficient of the surface, where 0 is opaque and 1 is fully
  //! transparent. Transparency is applicable to materials that have at least one of reflection
  //! modes (ambient, diffuse, specular or emissive) enabled. See also SetReflectionModeOn() and
  //! SetReflectionModeOff() methods.
  //!
  //! Warning: Raises MaterialDefinitionError if given value is a negative value or greater
  //! than 1.0.
  Standard_EXPORT void SetTransparency(const Standard_ShortReal theValue);

  //! Modifies the alpha coefficient of the surface, where 1.0 is opaque and 0.0 is fully
  //! transparent.
  void SetAlpha(Standard_ShortReal theValue) { SetTransparency(1.0f - theValue); }

  //! Returns the ambient color of the surface.
  const Quantity_Color& AmbientColor() const { return myColors[Graphic3d_TOR_AMBIENT]; }

  //! Modifies the ambient color of the surface.
  Standard_EXPORT void SetAmbientColor(const Quantity_Color& theColor);

  //! Returns the diffuse color of the surface.
  const Quantity_Color& DiffuseColor() const { return myColors[Graphic3d_TOR_DIFFUSE]; }

  //! Modifies the diffuse color of the surface.
  Standard_EXPORT void SetDiffuseColor(const Quantity_Color& theColor);

  //! Returns the specular color of the surface.
  const Quantity_Color& SpecularColor() const { return myColors[Graphic3d_TOR_SPECULAR]; }

  //! Modifies the specular color of the surface.
  Standard_EXPORT void SetSpecularColor(const Quantity_Color& theColor);

  //! Returns the emissive color of the surface.
  const Quantity_Color& EmissiveColor() const { return myColors[Graphic3d_TOR_EMISSION]; }

  //! Modifies the emissive color of the surface.
  Standard_EXPORT void SetEmissiveColor(const Quantity_Color& theColor);

  //! Returns the luminosity of the surface.
  Standard_ShortReal Shininess() const { return myShininess; }

  //! Modifies the luminosity of the surface.
  //! Warning: Raises MaterialDefinitionError if given value is a negative value or greater
  //! than 1.0.
  Standard_EXPORT void SetShininess(const Standard_ShortReal theValue);

  //! Increases or decreases the luminosity.
  //! @param theDelta a signed percentage
  Standard_EXPORT void IncreaseShine(const Standard_ShortReal theDelta);

  //! Returns the refraction index of the material
  Standard_ShortReal RefractionIndex() const { return myRefractionIndex; }

  //! Modifies the refraction index of the material.
  //! Warning: Raises MaterialDefinitionError if given value is a lesser than 1.0.
  Standard_EXPORT void SetRefractionIndex(const Standard_ShortReal theValue);

  //! Returns BSDF (bidirectional scattering distribution function).
  const Graphic3d_BSDF& BSDF() const { return myBSDF; }

  //! Modifies the BSDF (bidirectional scattering distribution function).
  void SetBSDF(const Graphic3d_BSDF& theBSDF) { myBSDF = theBSDF; }

  //! Returns physically based representation of material
  const Graphic3d_PBRMaterial& PBRMaterial() const { return myPBRMaterial; }

  //! Modifies the physically based representation of material
  void SetPBRMaterial(const Graphic3d_PBRMaterial& thePBRMaterial)
  {
    myPBRMaterial = thePBRMaterial;
  }

  //! Returns TRUE if the reflection mode is active, FALSE otherwise.
  Standard_Boolean ReflectionMode(const Graphic3d_TypeOfReflection theType) const
  {
    return !myColors[theType].IsEqual(Quantity_NOC_BLACK);
  }

  //! Returns material type.
  Graphic3d_TypeOfMaterial MaterialType() const { return myMaterialType; }

  //! Returns TRUE if type of this material is equal to specified type.
  Standard_Boolean MaterialType(const Graphic3d_TypeOfMaterial theType) const
  {
    return myMaterialType == theType;
  }

  //! Set material type.
  Standard_EXPORT void SetMaterialType(const Graphic3d_TypeOfMaterial theType);

  //! Returns TRUE if this material differs from specified one.
  Standard_Boolean IsDifferent(const Graphic3d_MaterialAspect& theOther) const
  {
    return !IsEqual(theOther);
  }

  //! Returns TRUE if this material differs from specified one.
  Standard_Boolean operator!=(const Graphic3d_MaterialAspect& theOther) const
  {
    return IsDifferent(theOther);
  }

  //! Returns TRUE if this material is identical to specified one.
  Standard_Boolean IsEqual(const Graphic3d_MaterialAspect& theOther) const
  {
    return myTransparencyCoef == theOther.myTransparencyCoef
           && myRefractionIndex == theOther.myRefractionIndex && myBSDF == theOther.myBSDF
           && myPBRMaterial == theOther.myPBRMaterial && myShininess == theOther.myShininess
           && myColors[Graphic3d_TOR_AMBIENT] == theOther.myColors[Graphic3d_TOR_AMBIENT]
           && myColors[Graphic3d_TOR_DIFFUSE] == theOther.myColors[Graphic3d_TOR_DIFFUSE]
           && myColors[Graphic3d_TOR_SPECULAR] == theOther.myColors[Graphic3d_TOR_SPECULAR]
           && myColors[Graphic3d_TOR_EMISSION] == theOther.myColors[Graphic3d_TOR_EMISSION];
  }

  //! Returns TRUE if this material is identical to specified one.
  Standard_Boolean operator==(const Graphic3d_MaterialAspect& theOther) const
  {
    return IsEqual(theOther);
  }

  //! Dumps the content of me into the stream
  Standard_EXPORT void DumpJson(Standard_OStream& theOStream, Standard_Integer theDepth = -1) const;

public:
  //! Deactivates the reflective properties of the surface with specified reflection type.
  Standard_DEPRECATED("Deprecated method, specific material component should be zerroed "
                      "instead")

  void SetReflectionModeOff(const Graphic3d_TypeOfReflection theType)
  {
    if (!ReflectionMode(theType))
    {
      return;
    }

    switch (theType)
    {
      case Graphic3d_TOR_AMBIENT:
        SetAmbientColor(Quantity_NOC_BLACK);
        break;
      case Graphic3d_TOR_DIFFUSE:
        SetDiffuseColor(Quantity_NOC_BLACK);
        break;
      case Graphic3d_TOR_SPECULAR:
        SetSpecularColor(Quantity_NOC_BLACK);
        break;
      case Graphic3d_TOR_EMISSION:
        SetEmissiveColor(Quantity_NOC_BLACK);
        break;
    }
  }

private:
  //! Initialize the standard material.
  Standard_EXPORT void init(const Graphic3d_NameOfMaterial theName);

  //! Mark material as user defined.
  void setUserMaterial()
  {
    // if a component of a "standard" material change, the
    // result is no more standard (a blue gold is not a gold)
    if (myMaterialName != Graphic3d_NameOfMaterial_UserDefined)
    {
      myMaterialName = Graphic3d_NameOfMaterial_UserDefined;
      myStringName   = "UserDefined";
    }
  }

private:
  Graphic3d_BSDF          myBSDF;
  Graphic3d_PBRMaterial   myPBRMaterial;
  TCollection_AsciiString myStringName;
  Quantity_Color          myColors[Graphic3d_TypeOfReflection_NB];
  Standard_ShortReal      myTransparencyCoef;
  Standard_ShortReal      myRefractionIndex;
  Standard_ShortReal      myShininess;

  Graphic3d_TypeOfMaterial myMaterialType;
  Graphic3d_NameOfMaterial myMaterialName;
  Graphic3d_NameOfMaterial myRequestedMaterialName;
};

#endif // _Graphic3d_MaterialAspect_HeaderFile
