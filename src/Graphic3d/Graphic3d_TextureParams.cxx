// Copyright (c) 2013-2014 OPEN CASCADE SAS
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


#include <Graphic3d_TextureParams.hxx>
#include <Standard_Type.hxx>

// =======================================================================
// function : Graphic3d_TextureParams
// purpose  :
// =======================================================================
Graphic3d_TextureParams::Graphic3d_TextureParams()
: myToModulate (Standard_False),
  myToRepeat   (Standard_False),
  myFilter     (Graphic3d_TOTF_NEAREST),
  myAnisoLevel (Graphic3d_LOTA_OFF),
  myRotAngle   (0.0f),
  myScale      (1.0f, 1.0f),
  myTranslation(0.0f, 0.0f),
  myGenMode    (Graphic3d_TOTM_MANUAL),
  myGenPlaneS  (0.0f, 0.0f, 0.0f, 0.0f),
  myGenPlaneT  (0.0f, 0.0f, 0.0f, 0.0f)
{
  //
}

// =======================================================================
// function : Destroy
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::Destroy() const
{
  //
}

// =======================================================================
// function : IsModulate
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureParams::IsModulate() const
{
  return myToModulate;
}

// =======================================================================
// function : SetModulate
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetModulate (const Standard_Boolean theToModulate)
{
  myToModulate = theToModulate;
}

// =======================================================================
// function : IsRepeat
// purpose  :
// =======================================================================
Standard_Boolean Graphic3d_TextureParams::IsRepeat() const
{
  return myToRepeat;
}

// =======================================================================
// function : SetRepeat
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetRepeat (const Standard_Boolean theToRepeat)
{
  myToRepeat = theToRepeat;
}

// =======================================================================
// function : Filter
// purpose  :
// =======================================================================
Graphic3d_TypeOfTextureFilter Graphic3d_TextureParams::Filter() const
{
  return myFilter;
}

// =======================================================================
// function : SetFilter
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetFilter (const Graphic3d_TypeOfTextureFilter theFilter)
{
  myFilter = theFilter;
}

// =======================================================================
// function : AnisoFilter
// purpose  :
// =======================================================================
Graphic3d_LevelOfTextureAnisotropy Graphic3d_TextureParams::AnisoFilter() const
{
  return myAnisoLevel;
}

// =======================================================================
// function : SetAnisoFilter
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetAnisoFilter (const Graphic3d_LevelOfTextureAnisotropy theLevel)
{
  myAnisoLevel = theLevel;
}

// =======================================================================
// function : Rotation
// purpose  :
// =======================================================================
Standard_ShortReal Graphic3d_TextureParams::Rotation() const
{
  return myRotAngle;
}

// =======================================================================
// function : SetRotation
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetRotation (const Standard_ShortReal theAngleDegrees)
{
  myRotAngle = theAngleDegrees;
}

// =======================================================================
// function : Scale
// purpose  :
// =======================================================================
const Graphic3d_Vec2& Graphic3d_TextureParams::Scale() const
{
  return myScale;
}

// =======================================================================
// function : SetScale
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetScale (const Graphic3d_Vec2 theScale)
{
  myScale = theScale;
}

// =======================================================================
// function : Translation
// purpose  :
// =======================================================================
const Graphic3d_Vec2& Graphic3d_TextureParams::Translation() const
{
  return myTranslation;
}

// =======================================================================
// function : SetTranslation
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetTranslation (const Graphic3d_Vec2 theVec)
{
  myTranslation = theVec;
}

// =======================================================================
// function : GenMode
// purpose  :
// =======================================================================
Graphic3d_TypeOfTextureMode Graphic3d_TextureParams::GenMode() const
{
  return myGenMode;
}

// =======================================================================
// function : GenPlaneS
// purpose  :
// =======================================================================
const Graphic3d_Vec4& Graphic3d_TextureParams::GenPlaneS() const
{
  return myGenPlaneS;
}

// =======================================================================
// function : GenPlaneT
// purpose  :
// =======================================================================
const Graphic3d_Vec4& Graphic3d_TextureParams::GenPlaneT() const
{
  return myGenPlaneT;
}

// =======================================================================
// function : SetGenMode
// purpose  :
// =======================================================================
void Graphic3d_TextureParams::SetGenMode (const Graphic3d_TypeOfTextureMode theMode,
                                          const Graphic3d_Vec4              thePlaneS,
                                          const Graphic3d_Vec4              thePlaneT)
{
  myGenMode   = theMode;
  myGenPlaneS = thePlaneS;
  myGenPlaneT = thePlaneT;
}
