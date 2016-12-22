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

//! OpenGL material definition
struct OpenGl_Material
{

  OpenGl_Vec4 Ambient;  //!< ambient reflection coefficient
  OpenGl_Vec4 Diffuse;  //!< diffuse reflection coefficient
  OpenGl_Vec4 Specular; //!< glossy  reflection coefficient
  OpenGl_Vec4 Emission; //!< material emission
  OpenGl_Vec4 Params;   //!< extra packed parameters

  float  Shine()        const { return Params.x(); }
  float& ChangeShine()        { return Params.x(); }

  float  Transparency() const { return Params.y(); }
  float& ChangeTransparency() { return Params.y(); }

  //! Set material color.
  void SetColor (const OpenGl_Vec4& theColor)
  {
    // apply the same formula as in Graphic3d_MaterialAspect::SetColor()
    Ambient.xyz() = theColor.rgb() * 0.25f;
    Diffuse.xyz() = theColor.rgb();
  }

  //! Initialize material
  void Init (const Graphic3d_MaterialAspect& theProp,
             const Quantity_Color&           theInteriorColor);

  //! Returns packed (serialized) representation of material properties
  const OpenGl_Vec4* Packed() const { return reinterpret_cast<const OpenGl_Vec4*> (this); }
  static Standard_Integer NbOfVec4() { return 5; }

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

};

//! Material flag
enum OpenGl_MaterialFlag
{
  OpenGl_MaterialFlag_Front, //!< material for front faces
  OpenGl_MaterialFlag_Back   //!< material for back  faces
};

#endif // _OpenGl_Material_Header
