// Created on: 2011-09-20
// Created by: Sergey ZERCHANINOV
// Copyright (c) 2011-2013 OPEN CASCADE SAS
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

#ifndef _OpenGl_Material_Header
#define _OpenGl_Material_Header

#include <Graphic3d_MaterialAspect.hxx>
#include <OpenGl_Vec.hxx>

class OpenGl_Context;

//! OpenGL material definition
struct OpenGl_MaterialCommon
{

  OpenGl_Vec4 Diffuse;           //!< diffuse RGB coefficients + alpha
  OpenGl_Vec4 Emission;          //!< material RGB emission
  OpenGl_Vec4 SpecularShininess; //!< glossy  RGB coefficients + shininess
  OpenGl_Vec4 Ambient;           //!< ambient RGB coefficients

  float  Shine() const { return SpecularShininess.a(); }
  float& ChangeShine() { return SpecularShininess.a(); }

  //! Empty constructor.
  OpenGl_MaterialCommon() : Diffuse (1.0f), Emission (1.0f), SpecularShininess (1.0f, 1.0f, 1.0f, 0.0f), Ambient (1.0f) {}

  //! Set material color.
  void SetColor (const OpenGl_Vec3& theColor)
  {
    // apply the same formula as in Graphic3d_MaterialAspect::SetColor()
    Ambient.SetValues (theColor * 0.25f, Ambient.a());
    Diffuse.SetValues (theColor, Diffuse.a());
  }

};

//! OpenGL material definition
struct OpenGl_MaterialPBR
{

  OpenGl_Vec4 BaseColor;   //!< base color of PBR material with alpha component
  OpenGl_Vec4 EmissionIOR; //!< light intensity which is emitted by PBR material and index of refraction
  OpenGl_Vec4 Params;      //!< extra packed parameters

  float  Metallic()  const { return Params.b(); }
  float& ChangeMetallic()  { return Params.b(); }

  float  Roughness() const { return Params.g(); }
  float& ChangeRoughness() { return Params.g(); }

  //! Empty constructor.
  OpenGl_MaterialPBR() : BaseColor (1.0f), EmissionIOR (1.0f), Params  (1.0f, 1.0f, 1.0f, 1.0f) {}

  //! Set material color.
  void SetColor (const OpenGl_Vec3& theColor)
  {
    BaseColor.SetValues (theColor, BaseColor.a());
  }

};

//! OpenGL material definition
struct OpenGl_Material
{
  OpenGl_MaterialCommon Common[2];
  OpenGl_MaterialPBR    Pbr[2];

  //! Set material color.
  void SetColor (const OpenGl_Vec3& theColor)
  {
    Common[0].SetColor (theColor);
    Common[1].SetColor (theColor);
    Pbr[0].SetColor (theColor);
    Pbr[1].SetColor (theColor);
  }

  //! Initialize material
  void Init (const OpenGl_Context& theCtx,
             const Graphic3d_MaterialAspect& theFront,
             const Quantity_Color& theFrontColor,
             const Graphic3d_MaterialAspect& theBack,
             const Quantity_Color& theBackColor);

  //! Check this material for equality with another material (without tolerance!).
  bool IsEqual (const OpenGl_Material& theOther) const
  {
    return std::memcmp (this, &theOther, sizeof(OpenGl_Material)) == 0;
  }

  //! Check this material for equality with another material (without tolerance!).
  bool operator== (const OpenGl_Material& theOther)       { return IsEqual (theOther); }
  bool operator== (const OpenGl_Material& theOther) const { return IsEqual (theOther); }

  //! Check this material for non-equality with another material (without tolerance!).
  bool operator!= (const OpenGl_Material& theOther)       { return !IsEqual (theOther); }
  bool operator!= (const OpenGl_Material& theOther) const { return !IsEqual (theOther); }

  //! Returns packed (serialized) representation of common material properties
  const OpenGl_Vec4* PackedCommon() const { return reinterpret_cast<const OpenGl_Vec4*> (Common); }
  static Standard_Integer NbOfVec4Common() { return 4 * 2; }

  //! Returns packed (serialized) representation of PBR material properties
  const OpenGl_Vec4* PackedPbr() const { return reinterpret_cast<const OpenGl_Vec4*> (Pbr); }
  static Standard_Integer NbOfVec4Pbr() { return 3 * 2; }

private:

  //! Initialize material
  void init (const OpenGl_Context& theCtx,
             const Graphic3d_MaterialAspect& theMat,
             const Quantity_Color& theColor,
             const Standard_Integer theIndex);

};

//! Material flag
enum OpenGl_MaterialFlag
{
  OpenGl_MaterialFlag_Front, //!< material for front faces
  OpenGl_MaterialFlag_Back   //!< material for back  faces
};

#endif // _OpenGl_Material_Header
