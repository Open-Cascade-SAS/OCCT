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

#include <Standard.hxx>
#include <Standard_DefineAlloc.hxx>
#include <Standard_Handle.hxx>

#include <Standard_ShortReal.hxx>
#include <Quantity_Color.hxx>
#include <Standard_Boolean.hxx>
#include <Graphic3d_BSDF.hxx>
#include <Graphic3d_TypeOfMaterial.hxx>
#include <Graphic3d_NameOfMaterial.hxx>
#include <TCollection_AsciiString.hxx>
#include <Standard_Real.hxx>
#include <Graphic3d_TypeOfReflection.hxx>
#include <Standard_CString.hxx>
#include <Standard_Integer.hxx>
class Graphic3d_MaterialDefinitionError;
class Standard_OutOfRange;
class Quantity_Color;


//! This class allows the definition of the type of a surface.
//! Aspect attributes of a 3d face.
//! Keywords: Material, FillArea, Shininess, Ambient, Color, Diffuse,
//! Specular, Transparency, Emissive, ReflectionMode,
//! BackFace, FrontFace, Reflection, Absorbtion
class Graphic3d_MaterialAspect 
{
public:

  DEFINE_STANDARD_ALLOC

  
  //! Creates a material from default values.
  Standard_EXPORT Graphic3d_MaterialAspect();
  
  //! Creates a generic material calls <AName>
  Standard_EXPORT Graphic3d_MaterialAspect(const Graphic3d_NameOfMaterial AName);
  
  //! Increases or decreases the luminosity of <me>.
  //! <ADelta> is a signed percentage.
  Standard_EXPORT void IncreaseShine (const Standard_Real ADelta);
  
  //! Modifies the reflection properties of the surface.
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <AValue> is
  //! a negative value or greater than 1.0.
  Standard_EXPORT void SetAmbient (const Standard_Real AValue);
  
  //! Modifies the reflection properties of the surface.
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <AValue> is a
  //! negative value or greater than 1.0.
  Standard_EXPORT void SetDiffuse (const Standard_Real AValue);
  
  //! Modifies the reflection properties of the surface.
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <AValue> is a
  //! negative value or greater than 1.0.
  Standard_EXPORT void SetEmissive (const Standard_Real AValue);
  
  //! Modifies the luminosity of the surface.
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <AValue> is a
  //! negative value or greater than 1.0.
  Standard_EXPORT void SetShininess (const Standard_Real AValue);
  
  //! Modifies the reflection properties of the surface.
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <AValue> is a
  //! negative value or greater than 1.0.
  Standard_EXPORT void SetSpecular (const Standard_Real AValue);
  
  //! Modifies the transparency coefficient of the surface.
  //! <AValue> = 0. opaque. (default)
  //! <AValue> = 1. transparent.
  //! Transparency is applicable to materials that have at least
  //! one of reflection modes (ambient, diffuse, specular or emissive)
  //! enabled. See also SetReflectionModeOn() and SetReflectionModeOff() methods.
  //!
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <AValue> is a
  //! negative value or greater than 1.0.
  Standard_EXPORT void SetTransparency (const Standard_Real AValue);
  
  //! Modifies the refraction index of the material.
  //! Category: Methods to modify the class definition
  //! Warning: Raises MaterialDefinitionError if <theValue> is a
  //! lesser than 1.0.
  Standard_EXPORT void SetRefractionIndex (const Standard_Real theValue);
  
  //! Modifies the BSDF (bidirectional scattering distribution function).
  //! Category: Methods to modify the class definition
  Standard_EXPORT void SetBSDF (const Graphic3d_BSDF& theBSDF);
  
  //! Modifies the ambient and diffuse colour of the surface.
  //! Category: Methods to modify the class definition
  Standard_EXPORT void SetColor (const Quantity_Color& AColor);
  
  //! Modifies the ambient colour of the surface.
  Standard_EXPORT void SetAmbientColor (const Quantity_Color& AColor);
  
  //! Modifies the difuse colour of the surface.
  Standard_EXPORT void SetDiffuseColor (const Quantity_Color& AColor);
  
  //! Modifies the specular colour of the surface.
  Standard_EXPORT void SetSpecularColor (const Quantity_Color& AColor);
  
  //! Modifies the emissive colour of the surface.
  Standard_EXPORT void SetEmissiveColor (const Quantity_Color& AColor);
  
  //! Activates the reflective properties of the surface <AType>.
  //!
  //! TypeOfReflection : TOR_AMBIENT
  //! TOR_DIFFUSE
  //! TOR_SPECULAR
  //! TOR_EMISSION
  //! 1, 2, 3 or 4 types of reflection can be set for a given surface.
  Standard_EXPORT void SetReflectionModeOn (const Graphic3d_TypeOfReflection AType);
  
  //! Deactivates the reflective properties of
  //! the surface <AType>.
  //!
  //! TypeOfReflection : TOR_AMBIENT
  //! TOR_DIFFUSE
  //! TOR_SPECULAR
  //! TOR_EMISSION
  //! 1, 2, 3 or 4 types of reflection can be set off for a given surface.
  //! Disabling diffuse and specular reflectance is useful for efficient visualization
  //! of large amounts of data as definition of normals for graphic primitives is not needed
  //! when only "all-directional" reflectance is active.
  //!
  //! NOTE: Disabling all four reflection modes also turns off the following effects:
  //! 1. Lighting. Colors of primitives are not affected by the material properties when lighting is off.
  //! 2. Transparency.
  Standard_EXPORT void SetReflectionModeOff (const Graphic3d_TypeOfReflection AType);
  
  //! Set MyMaterialType to the value of parameter <AType>
  //!
  //! TypeOfMaterial :   MATERIAL_ASPECT
  //! MATERIAL_PHYSIC
  Standard_EXPORT void SetMaterialType (const Graphic3d_TypeOfMaterial AType);
  
  //! The current matarial become a "UserDefined" material.
  //! Set the name of the "UserDefined" material.
  Standard_EXPORT void SetMaterialName (const Standard_CString AName);
  
  Standard_EXPORT void SetEnvReflexion (const Standard_ShortReal AValue);
  
  //! Resets the material with the original values according to
  //! the material name but leave the current color values untouched
  //! for the material of type ASPECT.
  Standard_EXPORT void Reset();
  
  //! Returns the diffuse colour of the surface.
  Standard_EXPORT const Quantity_Color& Color() const;
  
  //! Returns the ambient colour of the surface.
  Standard_EXPORT const Quantity_Color& AmbientColor() const;
  
  //! Returns the diffuse colour of the surface.
  Standard_EXPORT const Quantity_Color& DiffuseColor() const;
  
  //! Returns the specular colour of the surface.
  Standard_EXPORT const Quantity_Color& SpecularColor() const;
  
  //! Returns the emissive colour of the surface.
  Standard_EXPORT const Quantity_Color& EmissiveColor() const;
  
  //! Returns the reflection properties of the surface.
  Standard_EXPORT Standard_Real Ambient() const;
  
  //! Returns the reflection properties of the surface.
  Standard_EXPORT Standard_Real Diffuse() const;
  
  //! Returns the reflection properties of the surface.
  Standard_EXPORT Standard_Real Specular() const;
  
  //! Returns the transparency coefficient of the surface.
  Standard_EXPORT Standard_Real Transparency() const;
  
  //! Returns the refraction index of the material
  Standard_EXPORT Standard_Real RefractionIndex() const;
  
  //! Returns BSDF (bidirectional scattering distribution function).
  Standard_EXPORT const Graphic3d_BSDF& BSDF() const;
  
  //! Returns the emissive coefficient of the surface.
  Standard_EXPORT Standard_Real Emissive() const;
  
  //! Returns the luminosity of the surface.
  Standard_EXPORT Standard_Real Shininess() const;
  
  //! Returns Standard_True if the reflection mode is active,
  //! Standard_False otherwise.
  Standard_EXPORT Standard_Boolean ReflectionMode (const Graphic3d_TypeOfReflection AType) const;
  
  //! Returns Standard_True if MyMaterialType equal the parameter AType,
  //! Standard_False otherwise.
  Standard_EXPORT Standard_Boolean MaterialType (const Graphic3d_TypeOfMaterial AType) const;
  
  Standard_EXPORT Standard_ShortReal EnvReflexion() const;
  
  //! Returns the material name.
  Standard_EXPORT Graphic3d_NameOfMaterial Name() const;
  
  //! Returns Standard_True if the materials <me> and
  //! <Other> are different.
  Standard_EXPORT Standard_Boolean IsDifferent (const Graphic3d_MaterialAspect& Other) const;
Standard_Boolean operator != (const Graphic3d_MaterialAspect& Other) const
{
  return IsDifferent(Other);
}
  
  //! Returns Standard_True if the materials <me> and
  //! <Other> are identical.
  Standard_EXPORT Standard_Boolean IsEqual (const Graphic3d_MaterialAspect& Other) const;
Standard_Boolean operator == (const Graphic3d_MaterialAspect& Other) const
{
  return IsEqual(Other);
}
  

  //! Returns the number of predefined textures.
  Standard_EXPORT static Standard_Integer NumberOfMaterials();
  

  //! Returns the name of the predefined material of rank <aRank>
  //! Trigger: when <aRank> is < 1 or > NumberOfMaterials.
  Standard_EXPORT static Standard_CString MaterialName (const Standard_Integer aRank);
  

  //! Returns the name of this material
  Standard_EXPORT Standard_CString MaterialName() const;
  

  //! Returns the type of the predefined material of rank <aRank>
  //! Trigger: when <aRank> is < 1 or > NumberOfMaterials.
  Standard_EXPORT static Graphic3d_TypeOfMaterial MaterialType (const Standard_Integer aRank);
  

  //! Returns the material for specified name or Graphic3d_NOM_DEFAULT if name is unknown.
  Standard_EXPORT static Graphic3d_NameOfMaterial MaterialFromName (const Standard_CString theName);




protected:





private:

  
  Standard_EXPORT void Init (const Graphic3d_NameOfMaterial AName);


  Standard_ShortReal myDiffuseCoef;
  Quantity_Color myDiffuseColor;
  Standard_Boolean myDiffuseActivity;
  Standard_ShortReal myAmbientCoef;
  Quantity_Color myAmbientColor;
  Standard_Boolean myAmbientActivity;
  Standard_ShortReal mySpecularCoef;
  Quantity_Color mySpecularColor;
  Standard_Boolean mySpecularActivity;
  Standard_ShortReal myEmissiveCoef;
  Quantity_Color myEmissiveColor;
  Standard_Boolean myEmissiveActivity;
  Standard_ShortReal myTransparencyCoef;
  Standard_ShortReal myRefractionIndex;
  Graphic3d_BSDF myBSDF;
  Standard_ShortReal myShininess;
  Standard_ShortReal myEnvReflexion;
  Graphic3d_TypeOfMaterial myMaterialType;
  Graphic3d_NameOfMaterial myMaterialName;
  Graphic3d_NameOfMaterial myRequestedMaterialName;
  TCollection_AsciiString myStringName;


};







#endif // _Graphic3d_MaterialAspect_HeaderFile
